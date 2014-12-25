<?php
/**
 * @file
 * Object for working with stored panels in DB.
 */

/**
 * Class FppBundlesDatabase.
 */
class FppBundlesDatabase {
  const BUNDLE_CLASS_NAME = '\FppBundle';

  /**
   * An instance of object.
   *
   * @var \FppBundlesDatabase
   */
  private static $instance;

  /**
   * Database schema for Drupal "hook_schema" usage.
   *
   * @var array
   */
  private $schema = array();

  /**
   * Table fields names, exclude indexes.
   *
   * @var array
   */
  private $fields = array();

  /**
   * An array where key is name the index and value - field with index.
   *
   * @var array
   */
  private $indexes = array(
    'primary' => '',
    'unique' => '',
  );

  /**
   * An array with all bundles created from admin interface.
   *
   * @var array
   */
  private $allBundles = array();

  /**
   * DB table name.
   *
   * @var string
   */
  private $table = '';

  /**
   * Create instance to work with DB table.
   *
   * @param string $table_name
   *   The name of table in DB.
   */
  private function __construct($table_name) {
    $this->table = $table_name;
    $not_null_varchar = array(
      'type' => 'varchar',
      'length' => 100,
      'not null' => TRUE,
    );

    $indexes = array(
      // Primary key - auto increment.
      'bid' => array(
        'type' => 'serial',
        'unsigned' => TRUE,
        'description' => 'Bundle ID',
      ),
      // Unique key - generated automatically.
      'machine' => $not_null_varchar + array(
        'description' => 'Machine name of the bundle',
      ),
    );

    $fields = array(
      // Input in UI. Required field on base of which
      // will be generated a machine name of the bundle.
      'name' => $not_null_varchar + array(
        'description' => 'Human name of the bundle',
      ),
      // Checkbox in UI. Needed for the indicated where entity
      // will be shown: in category or in general list.
      'level' => array(
        'type' => 'int',
        'not null' => TRUE,
        'description' => 'Indicates that the panel is "top level" or placed into category.',
      ),
      // Input in UI.
      'category' => $not_null_varchar + array(
        'description' => 'Optional field for store bundles in categories.',
      ),
    );

    $this->indexes = array_combine(array_keys($this->indexes), array_keys($indexes));
    $this->fields = array_keys($fields);

    // Create DB schema for the storage module data.
    $this->schema = array(
      $this->table => array(
        'fields' => $indexes + $fields,
        'primary key' => array($this->indexes['primary']),
        'unique keys' => array($this->indexes['unique'] => array($this->indexes['unique'])),
        'description' => 'Storage for Fieldable Panels Panes bundles',
      ),
    );
  }

  /**
   * Property getter.
   */
  public function getSchema() {
    return $this->schema;
  }

  /**
   * Get the bundle by one of the indexes in table.
   *
   * @param string $index_name
   *   An index name. Must be "primary" or "unique".
   * @param int|string $value
   *   An ID of the bundle.
   *
   * @return bool|array
   *   False - if nothing was found or an array with info about bundle.
   */
  public function getBundleByDbIndex($index_name, $value) {
    if (!isset($this->indexes[$index_name])) {
      return FALSE;
    }

    return $this->bundleSelectQuery()
      ->condition($this->indexes[$index_name], $value, '=')
      ->execute()
      ->fetchObject(self::BUNDLE_CLASS_NAME);
  }

  /**
   * Get all bundles from the DB.
   *
   * @param int $per_page
   *   Number of items per page.
   *
   * @return array
   *   An array where each item is an instance of the \FppBundle;
   */
  public function getAllBundles($per_page = 0) {
    if (!$this->allBundles) {
      $query = $this->bundleSelectQuery();

      if ($per_page && is_int($per_page)) {
        /* @var \PagerDefault $query */
        $query = $query->extend('PagerDefault');
        $query->limit($per_page);
      }

      $this->allBundles = $query->execute()->fetchAll(
        \PDO::FETCH_CLASS, self::BUNDLE_CLASS_NAME
      );

      /* @var \FppBundle $bundle */
      foreach ($this->allBundles as $i => $bundle) {
        $this->allBundles[$bundle->getMachineName()] = $bundle;
        unset($this->allBundles[$i]);
      }
    }

    return $this->allBundles;
  }

  /**
   * Get the bundle by machine name.
   *
   * @param string $machine_name
   *   Machine readable name of the bundle.
   *
   * @return \FppBundle|bool
   *   An instance ob the bundle object or false if it does not exist.
   */
  public function getBundleByMachineName($machine_name) {
    $bundles = $this->getAllBundles();

    return isset($bundles[$machine_name]) ? $bundles[$machine_name] : FALSE;
  }

  /**
   * Get machine names of all bundles.
   *
   * @return array
   *   An array where each item - an unique machine name of the bundle.
   */
  public function getAllBundlesMachineNames() {
    return array_keys($this->getAllBundles());
  }

  /**
   * Get all field names from DB table where bundles was stored.
   *
   * @return array
   *   An array with fields machine names.
   */
  public function getAllSchemaFields() {
    return array_values($this->fields + $this->indexes);
  }

  /**
   * Select FPP bundles from DB.
   *
   * @return \SelectQuery
   *   Database object for select query.
   */
  public function bundleSelectQuery() {
    return db_select($this->table)->fields($this->table, $this->getAllSchemaFields());
  }

  /**
   * Save values for the bundle and create it.
   *
   * @param array $bundle
   *   Bundle object.
   *
   * @return int
   *   Last insert ID.
   */
  public function insertBundle(array $bundle) {
    return $this->performAction(FPP_BUNDLES_CREATE_BUNDLE_ACTION, $bundle);
  }

  /**
   * Update existing bundle.
   *
   * @param array $bundle
   *   Bundle object.
   */
  public function updateBundle(array $bundle) {
    $this->performAction(FPP_BUNDLES_UPDATE_BUNDLE_ACTION, $bundle);
  }

  /**
   * Remove existing bundle.
   *
   * @param array $bundle
   *   Bundle object.
   */
  public function deleteBundle(array $bundle) {
    $this->performAction(FPP_BUNDLES_REMOVE_BUNDLE_ACTION, $bundle);
  }

  /**
   * Perform a database action over the bundle.
   *
   * @param string $action
   *   An action name. Can be a value the one of next constants:
   *   FPP_BUNDLES_CREATE_BUNDLE_ACTION - insert a new entry in DB;
   *   FPP_BUNDLES_UPDATE_BUNDLE_ACTION - update an existing entry.
   *   FPP_BUNDLES_REMOVE_BUNDLE_ACTION - delete an existing entry.
   * @param array $bundle
   *   Bundle data.
   *
   * @see db_insert()
   * @see db_update()
   * @see db_delete()
   *
   * @throws \Exception
   *   When query cannot be performed or an action is incorrect.
   *
   * @return \DatabaseStatementInterface|int
   *   An interface on "update" and "delete" actions
   *   or ID of a new entry on "insert" action.
   */
  public function performAction($action, array $bundle) {
    $action = "db_$action";

    if (!function_exists($action)) {
      throw \FppBundlesException::set('Incorrect action over the bundle!');
    }

    $query = call_user_func($action, $this->table);
    $index = $this->indexes['primary'];
    $bundle_id = 0;

    if (isset($bundle[$index])) {
      $bundle_id = $bundle[$index];
      unset($bundle[$index]);
    }

    if (!($query instanceof \DeleteQuery)) {
      /* @var \UpdateQuery|\InsertQuery $query */
      $query->fields($bundle);
    }

    if ($bundle_id && !($query instanceof \InsertQuery)) {
      /* @var \UpdateQuery|\DeleteQuery $query */
      $query->condition($index, $bundle_id, '=');
    }

    return $query->execute();
  }

  /**
   * Create an object representation of the bundle.
   *
   * @param array $data
   *   Keyed array with information about bundle.
   *   Required keys are: "name", "level", "machine", "category".
   *
   * @throws \FppBundlesException
   *   If property is absent but should be.
   *
   * @return array
   *   Bundle information.
   */
  public function prepareBundleData(array $data) {
    $save = array();

    foreach ($this->getAllSchemaFields() as $field) {
      if (isset($data[$field])) {
        $save[$field] = $data[$field];
      }
      elseif ($field !== $this->indexes['primary']) {
        throw \FppBundlesException::set('The property "!property" is not specified or does not exist, but it is necessary to create the bundle.', array(
          '!property' => $field,
        ));
      }
    }

    return $save;
  }

  /**
   * Create/get an instance of an object.
   *
   * @see self::__construct()
   *
   * @return \FppBundlesDatabase
   *   An object instance.
   */
  public static function instance($table_name) {
    if (!self::$instance) {
      self::$instance = new self($table_name);
    }

    return self::$instance;
  }
}
