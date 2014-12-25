<?php
/**
 * @file
 * An object that describes single Fieldable Panels Pane Bundle.
 */

/**
 * Class FppBundle.
 */
class FppBundle {
  /**
   * The bundle ID.
   *
   * @var int
   */
  private $bid = 0;

  /**
   * The human name of bundle.
   *
   * @var string
   */
  private $name = '';

  /**
   * Indicates that the panel should be placed in top level or not.
   *
   * @var bool
   */
  private $level = FALSE;

  /**
   * An unique bundle machine name.
   *
   * @var string
   */
  private $machine = '';

  /**
   * Category where was placed the bundle.
   *
   * @var string
   */
  private $category = '';

  /**
   * Property getter.
   */
  public function getBundleId() {
    return (int) $this->bid;
  }

  /**
   * Property getter.
   */
  public function getHumanName() {
    return $this->name;
  }

  /**
   * Property getter.
   */
  public function isTopLevelPanel() {
    return (bool) $this->level;
  }

  /**
   * Property getter.
   */
  public function getMachineName() {
    return $this->machine;
  }

  /**
   * Property getter.
   */
  public function getCategoryName() {
    return $this->category;
  }

  /**
   * Get object representation as an array.
   *
   * @return array
   *   Bundle fields, except the ID.
   */
  public function getAllFieldsAsArray() {
    $parameters = get_object_vars($this);
    // Delete the ID from the property set, because
    // it is an unique and plays no role.
    unset($parameters['bid']);

    return $parameters;
  }

  /**
   * Method for export object parameters as an keyed array.
   *
   * @return array
   *   Representation of the object as an array.
   */
  public function export() {
    return array($this->machine => $this->getAllFieldsAsArray());
  }
}
