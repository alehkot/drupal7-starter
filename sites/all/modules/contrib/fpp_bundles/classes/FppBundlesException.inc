<?php
/**
 * @file
 * FPP Bundles Exception.
 */

/**
 * Class FppBundlesException.
 */
class FppBundlesException extends \Exception {
  /**
   * Throw Drupal error and store it to "Recent log messages".
   *
   * @param string $message
   *   Text message with placeholders.
   * @param array $args
   *   An array of arguments for replacing the placeholders.
   *
   * @return self
   *   Exception, which can be thrown.
   */
  public static function set($message, array $args = array()) {
    $message = format_string($message, $args);
    $exception = new self($message);

    watchdog_exception('FPP Bundles', $exception, $message);

    return $exception;
  }
}
