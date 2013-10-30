<?php
/**
 *  2008 Erick Romero (erm) <erick@adalid-soft.com>
 *  Fertility
*/

/**
 * @file
 * User page callbacks for the test_im module.
 */

/**
 * Site-wide contact page.
 */

function fertility_main() {

  $output = drupal_get_form('fertility_form');
  return $output;
}


function fertility_form () {

  $form = array();

  $now = time();
  $today = array(
    'year' =>   intval(strftime('%Y', $now)),
    'month' =>   intval(strftime('%m', $now)),
    'day' =>   intval(strftime('%d', $now))
  );

  $today2 = sprintf('%s%s%s', $today['year'],$today['month'],$today['day']);

  $note .=  t("Algumas mulheres se dão conta de quando ovulam porque sentem determinadas sensações corporais. Mas, a ovulação não é tão óbvia: na verdade, existem ocasiões em que é difícil saber com certeza o período em que se está ovulando. A calculadora da ovulação que você encontrará abaixo, lhe ajudará a determinar quais são os dias do mês em que estará mais fértil.  Para obter melhores resultados, insira a data do primeiro dia dos seus três últimos ciclos menstruais");

  $form['today'] = array(
    '#type' => 'hidden',
    '#value' => $today2,
  );

  $form['notes'] = array(
    '#type' => 'item',
    '#title' => t("Planejamento de Fertilidade"),
    '#description' => $note,
  );

  $form['div1'] = array(
    '#type' => 'fieldset',
    '#title' => t('Data 1'),
    '#description'  => t("Caso não se recorde de todas as datas, insira a data do primeiro dia do seu último ciclo menstrual"),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  $form['div1']['date1'] = array(
    '#type' => 'date',
    '#title' => t("Data inicial do último período menstrual"),
    '#default_value' => $today,
    '#required' => TRUE,
  );

  $form['div2'] = array(
    '#type' => 'fieldset',
    '#title' => t('Data 2'),
    '#description'  => t("Com base em um ciclo menstrual médio de 28 dias, insira a data do seu período menstrual anterior:"),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['div2']['date2'] = array(
    '#type' => 'date',
    '#title' => t("Data do período menstrual anterior ao primeiro"),
    '#default_value' => $today,
  );


  $form['div3'] = array(
    '#type' => 'fieldset',
    '#title' => t('Data 3'),
    '#description'  => t("Para efetuar um cálculo mais preciso, insira a data do seu período menstrual anterior (aproximadamente 3 meses atrás)"),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['div3']['date3'] = array(
    '#type' => 'date',
    '#title' => t("Data do período menstrual anterior"),
    '#default_value' => $today,
  );


  $form['email'] = array(
    '#type' => 'textfield',
    '#title' => t("E-mail"),
    '#size' => 20,
    '#description'  => t("Digite um e-mail para enviar os resultados"),
  );

//erm. submit
//----------------------------------------------------------------------------
  $form['submit'] = array(
  '#type'   => 'submit',
  '#value'  => t("Calcular"),
  );

  return $form;
}



function fertility_form_validate ($form, &$form_state) {

  $cycle = 28;
  $div = 0;
  $per1 = 0;
  $per2 = 0;
  $per3 = 0;

  $date1= mktime(0, 0, 0,
    $form_state['values']['date1']['month'],
    $form_state['values']['date1']['day'],
    $form_state['values']['date1']['year']);

  $date2= mktime(0, 0, 0,
    $form_state['values']['date2']['month'],
    $form_state['values']['date2']['day'],
    $form_state['values']['date2']['year']);

  $date3= mktime(0, 0, 0,
    $form_state['values']['date3']['month'],
    $form_state['values']['date3']['day'],
    $form_state['values']['date3']['year']);

  $date22 = sprintf('%s%s%s', $form_state['values']['date2']['year'],$form_state['values']['date2']['month'],$form_state['values']['date2']['day']);
  $date33 = sprintf('%s%s%s', $form_state['values']['date3']['year'],$form_state['values']['date3']['month'],$form_state['values']['date3']['day']);

  //se especifico la fecha dos
  if ($form_state['values']['today'] != $date22) {
    $div++;
    if ($date2 >= $date1){
      form_set_error('date2', t('Data 2 deve ser inferior a Data 1'));
    }
    else {
      $per1 = $date1 - $date2;
    }
  }
  else {
    $date2 = 0;
  }

  //se especifico la fecha dos y la fecha tres
  if ($form_state['values']['today'] != $date22 && $form_state['values']['today'] != $date33) {
    $div++;
    if ($date3 >= $date2) {
      form_set_error('date3', t('Data 3 deve ser inferior a Data 2'));
    }
    else {
      $per2 = $date2 - $date3;
    }
  }

  //se especifico la fecha tres
  if ($form_state['values']['today'] != $date33) {
    $div += 2;
    if ($date3 >= $date1) {
      form_set_error('date3', t('Data 3 deve ser inferior a Data 1'));
    }
    else {
      $per3 = $date1 - $date3;
    }
  }
  else {
    $date3 = 0;
  }

  if ( $div != 0) {
    $period = ($per1 + $per2 + $per3) / 86400;
    $cycle = round($period / $div, 0);
    if ($cycle < 21 || $cycle > 35) {
      $which = 'date1';
      if ($form_state['values']['today'] != $date22 && ($per1/86400 < 21 || $per1/86400 > 35)) {
        $which = 'date2';
      }
      elseif ($form_state['values']['today'] != $date33) {
        $which = 'date3';
      }
      form_set_error($which, t('Não podemos fazer o cálculo para períodos inferiores a 21 dias ou superior a 35'));
    }
  }

  $per_nex = mktime(0,0,0, strftime('%m', $date1), (strftime('%d', $date1)+($cycle-1)), strftime('%Y', $date1)); //next cycle

  $fer_end = mktime(0,0,0, strftime('%m', $per_nex), (strftime('%d', $per_nex)+(-14)), strftime('%Y', $per_nex)); //next cycle
  $fer_ini = mktime(0,0,0, strftime('%m', $fer_end), (strftime('%d', $fer_end)+(-5)), strftime('%Y', $fer_end)); //next cycle

  //$fer_end = $per_nex - (14 * 86400); //fertility period ends
  //$fer_ini = $fer_end - (5 * 86400);    //fertility period begins

  $params = array (
    'd0' => $per_nex,
    'd0c' => $cycle,
    'd01' => $fer_ini,
    'd02' => $fer_end,

    'd1' => $date1,
    'd2' => $date2,
    'd3' => $date3
  );

  //$form_state['values']['email'] = trim($form_state['values']['email']);
  if ( $form_state['values']['email'] <> '' && ! valid_email_address($form_state['values']['email']) ) {
    form_set_error('email', t('Digite um endereço de email válido'));
  }

  $form_state['params'] = sprintf(base64_encode(json_encode($params)));

}


function fertility_form_submit($form, &$form_state) {

  $target_email = $form_state['values']['email'];
  //$target_email = 'erick@adalid-soft.com';

  if ($target_email <> '') {

    db_query('SELECT id FROM {fertility} WHERE email = "%s"', $target_email);
    if ( ! db_affected_rows()) {
      db_query("INSERT INTO {fertility} (email, counter) VALUES ('%s', 1)", $target_email);
    }
    else {
      db_query("UPDATE {fertility} SET counter = counter + 1 ", $target_email);
    }


    $tmp_logo = sprintf('
      <img id="logo" alt="%s" src="%s/logo.png"/>
      <div>%s - %s</div>
      ',
      variable_get('site_name', ''),
      path_to_theme(),
      variable_get('site_name', ''),
      drupal_get_title()
    );


    //erm class to send mail with attachments easily
    require("class.phpmailer.php");
    $mail = new phpmailer();

    $mail->AddAddress($target_email); //erm defined at begin of this file

    $mail->From     = "notreply@{$_SERVER['SERVER_NAME']}";
    $mail->FromName = t("Fertility Calculator");
    $mail->Subject  = t("Fertility Calculator - Seus Resultados");
    $mail->Body     = $tmp_logo . fertility_result($form_state['params']);
    $mail->CharSet  = "UTF-8";
    $mail->IsHTML(TRUE);

    $mail->Send();

    drupal_set_message(t('O resultado foi enviado para seu e-mail'));

  }

  variable_set('fertility_counter', variable_get('fertility_counter', 0) + 1);

  flood_register_event('fertility');
  $form_state['redirect'] = 'fertility/result/' . $form_state['params'];

}

function fertility_result($params) {

  $printfriendly = sprintf('<div> %s </div>', t('<a target="_blank" href="@link">Printer Friendly</a>', array('@link' => url('fertility/print/' . $params))));

  $params   = json_decode(base64_decode($params), TRUE);

  $content  = '';

  if ( ! is_array($params) || count($params) != 7) {
    $content .= t("Invalid params");
  }
  else {
    $content .= fertility_format_result($params);
  }

  $content .= $printfriendly;

  return $content;
}

function fertility_print($params) {

  $params   = json_decode(base64_decode($params), TRUE);

  $content  = '';

  if ( ! is_array($params) || count($params) != 7) {
    $content .= t("Invalid params");
  }
  else {
    $content .= fertility_format_result($params);
  }

  $tmp_logo = sprintf('
    <img id="logo" alt="%s" src="%s/logo.png"/>
    <div>%s - %s</div>
    ',
    variable_get('site_name', ''),
    path_to_theme(),
    variable_get('site_name', ''),
    drupal_get_title()
  );

  $output = sprintf('
    <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
    <html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $language->language; ?>" xml:lang="<?php print $language->language; ?>">
      <head>
        <link rel="shortcut icon" href="misc/favicon.ico" type="image/x-icon" />
        %s
        <link type="text/css" rel="stylesheet" href="%s/print.css" />
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
    ',
    drupal_get_html_head(),
    path_to_theme(),
    drupal_get_title(),
    variable_get('site_name', ''),
    $tmp_logo,
    $content
  );

  echo $output;

}

function fertility_format_result($params) {

  $dates = explode('-', $dates);
  $dateformat = "%B %e, %Y";

  $content = '';

  $content .=  sprintf('<h3> %s </h3>', t("Datas selecionadas"));

  if ($params['d1'] != 0) {
    $content .= sprintf('%s: %s<br />', t("Data 1"), strftime($dateformat, $params['d1']));
  }

  if ($params['d2'] != 0) {
    $content .= sprintf('%s: %s<br />', t("Data 2"), strftime($dateformat, $params['d2']));
  }

  if ($params['d3'] != 0) {
    $content .= sprintf('%s: %s<br />', t("Data 3"), strftime($dateformat, $params['d3']));
  }

  $content .=  "<br />";

  $content .=  sprintf('%s <b> %s </b> %s <br />', t("Seu ciclo menstrual é de aproximadamente: "), $params['d0c'], t("dias "));
  $content .=  sprintf('%s <b> %s </b> <br /><br />', t("A data do próximo período menstrual deverá ser: "), strftime($dateformat, $params['d0']));

  $content .=  sprintf('<h3 > %s </h3>', t("Informação relacionada com o período de fertilidade:"));

  $content .=  sprintf('<b> %s </b> %s <br />', strftime($dateformat, $params['d01']), t("Alta fertilidade. Começa em período de alta fertilidade"));
  $content .=  sprintf('<b> %s </b> %s <br />', strftime($dateformat, $params['d02']), t("Ovulação . Termina em período de alta fertilidade"));

  //erm generate the calendars
  $month1 = array();
  $month2 = array();
  $month  = intval(strftime('%m', $params['d01']));
  $year  = intval(strftime('%Y', $params['d01']));
  $date   = $params['d01'];

  $config = array (
     'local_time'   => $params['d0'],
     'template'     =>
      '{table_open}<table class="cal_table" border="0" cellpadding="2" cellspacing="2">{/table_open}

       {cal_cell_content} <div class="cal_fertil" > <b>{day}</b> </div> {/cal_cell_content}
       {cal_cell_content_today} <div class="cal_next" > <b>{day}</b> </div>{/cal_cell_content_today}
      '
   );

  require("class.calendar.php");
  $calendar = new CI_Calendar($config);

  //erm dias del periodo fertil
  while ($date <= $params['d02']) {
    $day = intval(strftime('%d', $date));
    $mm = intval(strftime('%m', $date));
    if ($mm == $month) {
      $month1[$day] = $day;
    }
    else {
      $month2[$day] = $day;
    }
    //$date += 86400;
    $date = mktime(0,0,0, strftime('%m', $date), (strftime('%d', $date)+1), strftime('%Y', $date)); //next cycle
  }

  //erm. dia del proximo periodo
  $day = intval(strftime('%d', $params['d0']));
  $mm = intval(strftime('%m', $params['d0']));
  if ($mm == $month) {
    $month1[$day] = $day;
  }
  else {
    $month2[$day] = $day;
  }

  $content .= '
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
    .calendar td {
      text-align: center;
    }
    .cal_table {
      width: 200px;
      background: #E6E6FA;
      border: 0px solid #000000;
    }
    .cal_fertil {
      background: #FFDADA;
      color: red;
      border: 1px solid red;
      padding: 1px 2px 1px 2px;
    }
    .cal_next {
      background: #D9FFD9;
      color: green;
      border: 1px solid green;
      padding: 1px 2px 1px 2px;
    }
    </style>
  ';
  $content .= '<div class="calendar">';

  $content .= '<table style="border: 0px solid black; width: 500px;" ><tr>';
  if (count($month1)) {
    $content .= sprintf('<td valign="top"> %s </td>', $calendar->generate($year, $month, $month1));
  }
  if (count($month2)) {
    $content .=  sprintf('<td valign="top"> %s </td>', $calendar->generate($year, $month+1, $month2));
  }
  $content .= '</tr>';

  $content .=  sprintf('<tr><td colspan="2"> <div> %s </div> <div> %s </div></td></tr>',
    t("Vermelho: Período de alta fertilidade e ovulação"),
    t("Verde: Data provável do próximo ciclo menstrual")
  );

  $content .= '</table></div>';

  $content .=  sprintf('<hr /> <b> %s </b>', t("ADVERTÊNCIA: Este calendário não pode e nem deve ser usado para prevenir gravidez"));

  return $content;

}
