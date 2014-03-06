<?php

/**
 * @file
 * Administration page callbacks for Moodle2 Integration.
 */

/**
 * Implements hook_admin_settings().
 */
function moodle2_admin_settings() {

  // Moodle Database Settings.
  $form['teacher_role_id'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Teacher role ID'),
    '#default_value' => variable_get('teacher_role_id', 4),
    '#size'          => 2,
    '#required'      => TRUE,
  );
  $form['course_manager_role_id'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Course manager role ID'),
    '#default_value' => variable_get('course_manager_role_id', 6),
    '#size'          => 2,
    '#required'      => TRUE,
  );
  $form['profile_student_profile'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Machine name of student profile in profile2  type'),
    '#default_value' => variable_get('profile_student_profile', 'profile_student_profile'),
    '#description'   => t('Default value:profile_student_profile'),
    '#size'          => 30,
    '#required'      => TRUE,
  );

  return system_settings_form($form);
}
