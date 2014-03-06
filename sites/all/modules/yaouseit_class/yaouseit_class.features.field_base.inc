<?php
/**
 * @file
 * yaouseit_class.features.field_base.inc
 */

/**
 * Implements hook_field_default_field_bases().
 */
function yaouseit_class_field_default_field_bases() {
  $field_bases = array();

  // Exported field_base: 'field_class_lesson'
  $field_bases['field_class_lesson'] = array(
    'active' => '1',
    'cardinality' => '1',
    'deleted' => '0',
    'entity_types' => array(),
    'field_name' => 'field_class_lesson',
    'foreign keys' => array(
      'tid' => array(
        'columns' => array(
          'tid' => 'tid',
        ),
        'table' => 'taxonomy_term_data',
      ),
    ),
    'indexes' => array(
      'tid' => array(
        0 => 'tid',
      ),
    ),
    'locked' => '0',
    'module' => 'taxonomy',
    'settings' => array(
      'allowed_values' => array(
        0 => array(
          'vocabulary' => 'jiao_cai_courses',
          'parent' => '0',
        ),
      ),
      'options_list_callback' => 'i18n_taxonomy_allowed_values',
      'profile2_private' => FALSE,
    ),
    'translatable' => '0',
    'type' => 'taxonomy_term_reference',
  );

  // Exported field_base: 'field_course_schedule'
  $field_bases['field_course_schedule'] = array(
    'active' => '1',
    'cardinality' => '1',
    'deleted' => '0',
    'entity_types' => array(),
    'field_name' => 'field_course_schedule',
    'foreign keys' => array(
      'tid' => array(
        'columns' => array(
          'tid' => 'tid',
        ),
        'table' => 'taxonomy_term_data',
      ),
    ),
    'indexes' => array(
      'tid' => array(
        0 => 'tid',
      ),
    ),
    'locked' => '0',
    'module' => 'taxonomy',
    'settings' => array(
      'allowed_values' => array(
        0 => array(
          'vocabulary' => 'course_schedule',
          'parent' => '0',
        ),
      ),
      'options_list_callback' => 'i18n_taxonomy_allowed_values',
      'profile2_private' => FALSE,
    ),
    'translatable' => '0',
    'type' => 'taxonomy_term_reference',
  );

  // Exported field_base: 'field_student'
  $field_bases['field_student'] = array(
    'active' => '1',
    'cardinality' => '-1',
    'deleted' => '0',
    'entity_types' => array(),
    'field_name' => 'field_student',
    'foreign keys' => array(
      'uid' => array(
        'columns' => array(
          'uid' => 'uid',
        ),
        'table' => 'users',
      ),
    ),
    'indexes' => array(
      'uid' => array(
        0 => 'uid',
      ),
    ),
    'locked' => '0',
    'module' => 'user_reference',
    'settings' => array(
      'profile2_private' => FALSE,
      'referenceable_roles' => array(
        2 => 0,
        3 => 0,
        4 => 0,
        5 => '5',
        6 => 0,
      ),
      'referenceable_status' => array(
        0 => 0,
        1 => 0,
      ),
      'view' => array(
        'args' => array(),
        'display_name' => '',
        'view_name' => '',
      ),
    ),
    'translatable' => '0',
    'type' => 'user_reference',
  );

  // Exported field_base: 'field_teacher'
  $field_bases['field_teacher'] = array(
    'active' => '1',
    'cardinality' => '-1',
    'deleted' => '0',
    'entity_types' => array(),
    'field_name' => 'field_teacher',
    'foreign keys' => array(
      'uid' => array(
        'columns' => array(
          'uid' => 'uid',
        ),
        'table' => 'users',
      ),
    ),
    'indexes' => array(
      'uid' => array(
        0 => 'uid',
      ),
    ),
    'locked' => '0',
    'module' => 'user_reference',
    'settings' => array(
      'profile2_private' => FALSE,
      'referenceable_roles' => array(
        2 => 0,
        3 => 0,
        4 => 0,
        5 => 0,
        6 => '6',
      ),
      'referenceable_status' => array(
        0 => 0,
        1 => 0,
      ),
      'view' => array(
        'args' => array(),
        'display_name' => '',
        'view_name' => '',
      ),
    ),
    'translatable' => '0',
    'type' => 'user_reference',
  );

  return $field_bases;
}
