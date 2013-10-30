<?php
/**
 *  2009 Erick Romero (erm) <erick@adalid-soft.com>
 *  Gestation
 */

/**
 * @file
 * User page callbacks for the test_im module.
 */


// return the main form
/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gestation_main() {
  $output = drupal_get_form('gestation_form');
  return $output;
}

// create the main form to select date known
/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gestation_form($form) {

  $form = array();

  $note = t("Purpose: To estimate the duration of pregnancy using 266 days from the date of conception (or 280 days from the first day of the last menstrual period).");

  $form['notes'] = array(
    '#type' => 'item',
    '#title' => t("Gestation Calculator"),
    '#description' => $note,
  );

  $form['date_known'] = array(
    '#type' => 'radios',
    '#title' => t("Date known"),
    '#required' => TRUE,

    //'#default_value' => 1,
    '#options' => array(
      1 => t("First day of the last menstrual period (LMP)"),
      2 => t("Estimated date of confinement (EDC)"),
    ),
  );

  //erm. submit
  //----------------------------------------------------------------------------
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t("Continue"),
  );

  return $form;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gestation_form_submit($form, &$form_state) {
  if ($form_state['values']['date_known'] == 1) {
    $form_state['redirect'] = 'gestation/lmp/';
  }
  else if ($form_state['values']['date_known'] == 2) {
    $form_state['redirect'] = 'gestation/edc/';
  }
}


// return the lmp form
/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gestation_lmp() {
  $output = "";
  $output = drupal_get_form('gestation_lmp_form');
  return $output;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gestation_lmp_form($form) {

  $form = array();

  $note = t("Purpose: To estimate the duration of pregnancy using 266 days from the date of conception (or 280 days from the first day of the last menstrual period).");

  $form['notes'] = array(
    '#type' => 'item',
    '#title' => t("Gestation Calculator"),
    '#description' => $note,
  );

  $form['div1'] = array(
    '#type' => 'fieldset',
    '#title' => t("Date known"),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  $form['div1']['date1'] = array(
    '#type' => 'date',
    '#title' => t("First day of the last menstrual period (LMP)"),
    '#required' => TRUE,
  );

  $form['div1']['date2'] = array(
    '#type' => 'date',
    '#title' => t("Examination date"),
    '#required' => TRUE,
  );

  $form['email'] = array(
    '#type' => 'textfield',
    '#title' => t("E-mail"),
    '#size' => 20,
    '#description' => t("Enter an e-mail to get the results"),
  );

  //erm. submit
  //----------------------------------------------------------------------------
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t("Calcular"),
  );

  return $form;
}


/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gestation_lmp_form_validate($form, &$form_state) {

  $date1 = mktime(0, 0, 0, $form_state['values']['date1']['month'], $form_state['values']['date1']['day'], $form_state['values']['date1']['year']);

  $date2 = mktime(0, 0, 0, $form_state['values']['date2']['month'], $form_state['values']['date2']['day'], $form_state['values']['date2']['year']);

  if ($date2 - $date1 < 0) {
    form_set_error('date2', t("The examination date cannot be before LMP"));
  }

  if ($date2 - $date1 > (320 * 86400)) {
    form_set_error('date2', t("The exam date is far past the LMP"));
  }

  if ($form_state['values']['email'] <> '' && ! valid_email_address($form_state['values']['email'])) {
    form_set_error('email', t('Enter a valid e-email'));
  }

  $params = array();

  $params['t'] = 1;

  $params['d1'] = mktime(0, 0, 0, strftime('%m', $date1), strftime('%d', $date1), strftime('%Y', $date1));

  $params['d2'] = mktime(0, 0, 0, strftime('%m', $date2), strftime('%d', $date2), strftime('%Y', $date2));

  $params['d3'] = mktime(0, 0, 0, strftime('%m', $date1), strftime('%d', $date1) + 280, strftime('%Y', $date1));

  $params['d4'] = mktime(0, 0, 0, strftime('%m', $date1), strftime('%d', $date1) + 14, strftime('%Y', $date1));

  $params['w1'] = round((($date2 - $date1) / 86400) / 7, 2);
  $params['w2'] = round($params['w1'] - 2, 2);

  $form_state['params'] = base64_encode(json_encode($params));

}


/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gestation_lmp_form_submit($form, &$form_state) {

  if ($form_state['values']['email'] <> '') {
    gestation_send_email($form_state);
  }

  variable_set('gestation_counter', variable_get('gestation_counter', 0) + 1);

  flood_register_event('gestation');
  $form_state['redirect'] = 'gestation/result/' . $form_state['params'];

}




// return the edc form
/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gestation_edc() {
  $output = "";
  $output = drupal_get_form('gestation_edc_form');
  return $output;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gestation_edc_form($form) {

  $form = array();

  $note = t("Purpose: To estimate the duration of pregnancy using 266 days from the date of conception (or 280 days from the first day of the last menstrual period).");

  $form['notes'] = array(
    '#type' => 'item',
    '#title' => t("Gestation Calculator"),
    '#description' => $note,
  );

  $form['div1'] = array(
    '#type' => 'fieldset',
    '#title' => t("Date known"),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  $form['div1']['date1'] = array(
    '#type' => 'date',
    '#title' => t("Estimated date of confinement (EDC)"),
    '#required' => TRUE,
  );

  $form['div1']['date2'] = array(
    '#type' => 'date',
    '#title' => t("Examination date"),
    '#required' => TRUE,
  );

  $form['email'] = array(
    '#type' => 'textfield',
    '#title' => t("E-mail"),
    '#size' => 20,
    '#description' => t("Enter an e-mail to get the results"),
  );

  //erm. submit
  //----------------------------------------------------------------------------
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t("Calcular"),
  );

  return $form;
}


/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gestation_edc_form_validate($form, &$form_state) {

  $date1 = mktime(0, 0, 0, $form_state['values']['date1']['month'], $form_state['values']['date1']['day'], $form_state['values']['date1']['year']);

  $date2 = mktime(0, 0, 0, $form_state['values']['date2']['month'], $form_state['values']['date2']['day'], $form_state['values']['date2']['year']);

  if ($date1 - $date2 < (-60 * 86400)) {
    form_set_error('date2', t("EDC and Examination date may be incorrect"));
  }

  $params = array();

  $params['t'] = 2;

  $params['d1'] = mktime(0, 0, 0, strftime('%m', $date1), strftime('%d', $date1), strftime('%Y', $date1));

  $params['d2'] = mktime(0, 0, 0, strftime('%m', $date2), strftime('%d', $date2), strftime('%Y', $date2));

  $params['d3'] = mktime(0, 0, 0, strftime('%m', $date1), strftime('%d', $date1) - 280, strftime('%Y', $date1));

  $params['d4'] = mktime(0, 0, 0, strftime('%m', $date1), strftime('%d', $date1) - 266, strftime('%Y', $date1));

  $params['w1'] = round((($date2 - $params['d3']) / 86400) / 7, 2);
  $params['w2'] = round($params['w1'] - 2, 2);

  /*
  if ($params['w1'] < 0) {
    form_set_error('date2', t("EDC and Examination date may be incorrect"));
  }
*/

  $form_state['params'] = base64_encode(json_encode($params));

  if ($form_state['values']['email'] <> '' && ! valid_email_address($form_state['values']['email'])) {
    form_set_error('email', t('Enter a valid e-email'));
  }

}


/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gestation_edc_form_submit($form, &$form_state) {

  if ($form_state['values']['email'] <> '') {
    gestation_send_email($form_state);
  }

  variable_set('gestation_counter', variable_get('gestation_counter', 0) + 1);

  flood_register_event('gestation');
  $form_state['redirect'] = 'gestation/result/' . $form_state['params'];

}


/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gestation_send_email(&$form_state) {

  $target_email = $form_state['values']['email'];

  $result = db_query('SELECT * FROM {fertility} WHERE email = :email', array(':email' => $target_email));

  
  if ($result->rowCount()==0) {
    // TODO Please review the conversion of this statement to the D7 database API syntax.
    /* db_query("INSERT INTO {fertility} (email, counter) VALUES ('%s', 1)", $target_email) */
    db_insert('fertility')->fields(array(
      'email' => $target_email,
      'counter' => 1,
    ))->execute();
  }
  else {
    // TODO Please review the conversion of this statement to the D7 database API syntax.
    /* db_query("UPDATE {fertility} SET counter = counter + 1 ", $target_email) */
	$record = $result->fetchObject();
    db_update('fertility')->fields(array(
      'counter' => $record->counter + 1,
    ))->execute();
  }

  $tmp_logo = sprintf('
    <img id="logo" alt="" src="http://%s/%s/logo.png"/>
    <div>%s - %s</div>
    ', variable_get('site_name', ''), path_to_theme(), variable_get('site_name', ''), drupal_get_title());

  //erm class to send mail with attachments easily
  $path = drupal_get_path('module', 'gestation');
  
  require $path. '/' . "class.phpmailer.php";
  $mail = new phpmailer();

  $mail->AddAddress($target_email); //erm defined at begin of this file

  $mail->From = "notreply@{$_SERVER['SERVER_NAME']}";
  $mail->FromName = t("Gestation Calculator");
  $mail->Subject = t("Gestation Calculator - Seus Resultados");
  $mail->Body = $tmp_logo . gestation_result($form_state['params']);
  $mail->CharSet = "UTF-8";
  $mail->IsHTML(TRUE);

  $mail->Send();

  drupal_set_message(t('The results were sent to your e-mail'));

}


/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gestation_result($params) {

  $printfriendly = sprintf('<div> %s </div>', t('<a target="_blank" href="@link">Printer Friendly</a>', array('@link' => url('gestation/print/' . $params))));

  $params = json_decode(base64_decode($params), TRUE);

  $content = '';

  if (! is_array($params) || count($params) != 7) {
    $content .= t("Invalid params");
  }
  else {
    $content .= gestation_format_result($params);
    $content .= $printfriendly;
  }

  return $content;
}


/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gestation_format_result($params) {

  if ($params['t'] != 1 && $params['t'] != 2) {
    return t("Unkown selected type");
  }

  $content = '';
  $dateformat = "%B %d, %Y";

  $date1 = array();
  $date2 = array();

  if ($params['t'] == 1) {

    $content .= sprintf('<h3> %s </h3>', t("Selected Dates"));
    $content .= sprintf('<div> %s: %s </div>', t("First day of the last menstrual period (LMP)"), strftime($dateformat, $params['d1']));
    $content .= sprintf('<div> %s: %s </div>', t("Examination date"), strftime($dateformat, $params['d2']));
    $content .= '<br />';

    $content .= sprintf('<h3> %s </h3>', t("Results"));

    $content .= sprintf('<div> %s: <b> %s </b> %s </div>', t("Estimated date of confinement (EDC)"), strftime($dateformat, $params['d3']), t("+/- 2 weeks"));
    $content .= sprintf('<div> %s: <b> %s </b> </div>', t("Estimated date of conception"), strftime($dateformat, $params['d4']));
    $content .= '<br />';

    $content .= sprintf('<div> %s: %s </div>', t("Examination date"), strftime($dateformat, $params['d2']));
    $content .= sprintf('<div> %s: <b> %s </b> </div>', t("Weeks of gestation from LMP"), $params['w1']);
    $content .= sprintf('<div> %s: <b> %s </b> </div>', t("Weeks since conception"), $params['w2']);

    $date1['yy'] = intval(strftime('%Y', $params['d4']));
    $date1['mm'] = intval(strftime('%m', $params['d4']));
    $date1['dd'] = array(intval(strftime('%d', $params['d4'])) => sprintf('<div class="cal_red"> %s </div>', intval(strftime('%d', $params['d4']))));
    $date1['nn'] = t("Red: Estimated date of conception");

    $date2['yy'] = intval(strftime('%Y', $params['d3']));
    $date2['mm'] = intval(strftime('%m', $params['d3']));
    $date2['dd'] = array(intval(strftime('%d', $params['d3'])) => sprintf('<div class="cal_green"> %s </div>', intval(strftime('%d', $params['d3']))));
    $date2['nn'] = t("Green: Estimated date of confinement");

  }

  if ($params['t'] == 2) {

    $content .= sprintf('<h3> %s </h3>', t("Selected Dates"));
    $content .= sprintf('<div> %s: %s </div>', t("Estimated date of confinement (EDC)"), strftime($dateformat, $params['d1']));
    $content .= sprintf('<div> %s: %s </div>', t("Examination date"), strftime($dateformat, $params['d2']));
    $content .= '<br />';

    $content .= sprintf('<h3> %s </h3>', t("Results"));

    $content .= sprintf('<div> %s: <b> %s </b> </div>', t("First day of the last menstrual period (LMP)"), strftime($dateformat, $params['d3']));
    $content .= sprintf('<div> %s: <b> %s </b> </div>', t("Estimated date of conception"), strftime($dateformat, $params['d4']));
    $content .= '<br />';

    $content .= sprintf('<div> %s: %s </div>', t("Examination date"), strftime($dateformat, $params['d2']));
    if ($params['d2'] > $params['d1']) {
      $content .= sprintf('<div> <b> %s </b> </div>', t("The patient is post-dates"));
    }
    $content .= sprintf('<div> %s: <b> %s </b> </div>', t("Weeks of gestation from LMP"), $params['w1']);
    $content .= sprintf('<div> %s: <b> %s </b> </div>', t("Weeks since conception"), $params['w2']);

    $date2['yy'] = intval(strftime('%Y', $params['d4']));
    $date2['mm'] = intval(strftime('%m', $params['d4']));
    $date2['dd'] = array(intval(strftime('%d', $params['d4'])) => sprintf('<div class="cal_red"> %s </div>', intval(strftime('%d', $params['d4']))));
    $date2['nn'] = t("Red: Estimated date of conception");

    $date1['yy'] = intval(strftime('%Y', $params['d3']));
    $date1['mm'] = intval(strftime('%m', $params['d3']));
    $date1['dd'] = array(intval(strftime('%d', $params['d3'])) => sprintf('<div class="cal_green"> %s </div>', intval(strftime('%d', $params['d3']))));
    $date1['nn'] = t("Green: First day of the last menstrual period");

  }

  //erm generate the calendars
  $calendar = '';

  $config = array(
    'template' => '
       {table_open}<table class="cal_table" border="0" cellpadding="2" cellspacing="2">{/table_open}
       {cal_cell_content} <b>{content}</b> {/cal_cell_content}
       {cal_cell_content_today} <b>{content}</b> {/cal_cell_content_today}
     ',
  );

  $path = drupal_get_path('module', 'gestation');
  require $path . '/' . "class.calendar.php";
  $calendar_class = new CI_Calendar($config);

  $calendar .= '
    <style type="text/css">
    body {
      font-size: 12px;
    }
    h1 {
      font-size: 140%;
    }
    h2 {
      font-size: 130%;
    }
    h3 {
      font-size: 120%;
    }
    .calendar {
      text-align: center;
    }
    .calendar td {
      text-align: center;
    }
    .cal_table {
      width: 200px;
      background: #E6E6FA;
      border: 0px solid #000000;
    }
    .cal_red {
      background: #FFDADA;
      color: red;
      border: 1px solid red;
      padding: 1px 2px 1px 2px;
    }
    .cal_green {
      background: #D9FFD9;
      color: green;
      border: 1px solid green;
      padding: 1px 2px 1px 2px;
    }
    </style>
  ';
  $calendar .= '<div class="calendar">';

  $calendar .= '<table border=0 style="width: 500px;" ><tr>';

  $calendar .= sprintf('<td valign="top"> %s </td>', $calendar_class->generate($date1['yy'], $date1['mm'], $date1['dd']));
  $calendar .= '<td valign="middle"> <div style="border:1px; text-align:center;"><big><strong> ... </strong></big></div> </td>';
  $calendar .= sprintf('<td valign="top"> %s </td>', $calendar_class->generate($date2['yy'], $date2['mm'], $date2['dd']));

  $calendar .= '</tr>';

  $calendar .= sprintf('<tr><td colspan="3"> <div> %s </div> <div> %s </div></td></tr>', $date1['nn'], $date2['nn']);

  $calendar .= '</table></div>';
  $content .= $calendar;

  return $content;

}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function gestation_print($params) {

  $params = json_decode(base64_decode($params), TRUE);

  $content = '';

  if (! is_array($params) || count($params) != 7) {
    $content .= t("Invalid params");
  }
  else {
    $content .= gestation_format_result($params);
  }

  $tmp_logo = sprintf('
    <img id="logo" alt="" src="http://%s/%s/logo.png"/>
    <div>%s - %s</div>
    ', variable_get('site_name', ''), path_to_theme(), variable_get('site_name', ''), drupal_get_title());

  $output = sprintf('
    <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
    <html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $language->language; ?>" xml:lang="<?php print $language->language; ?>">
      <head>
        <link rel="shortcut icon" href="misc/favicon.ico" type="image/x-icon" />
        %s
        <link type="text/css" rel="stylesheet" href="%s/print.css" media="all"/>
        <title> %s | %s</title>
      </head>
      <body >
        <div style="text-align:left;">
          <h1>%s</h1>
          %s <br />
        </div>

        <script type="text/javascript">
          print();
        </script>

      </body>
    </html>
    ', drupal_get_html_head(), path_to_theme(), drupal_get_title(), variable_get('site_name', ''), $tmp_logo, $content);

  echo $output;

}
