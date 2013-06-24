<?php
/**
 * Implements hook_views_default_views().
 */
function uuke_alipay_views_default_views() {
 
	// exported view goes here
	{
		$view = new view();
		$view->name = 'uuke_order_list';
		$view->description = '';
		$view->tag = 'default';
		$view->base_table = 'node';
		$view->human_name = 'uuke Order LIst';
		$view->core = 7;
		$view->api_version = '3.0';
		$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

		/* Display: Master */
		$handler = $view->new_display('default', 'Master', 'default');
		$handler->display->display_options['title'] = 'All Order LIst';
		$handler->display->display_options['use_more_always'] = FALSE;
		$handler->display->display_options['access']['type'] = 'perm';
		$handler->display->display_options['access']['perm'] = 'administer uuke alipay configuration';
		$handler->display->display_options['cache']['type'] = 'none';
		$handler->display->display_options['query']['type'] = 'views_query';
		$handler->display->display_options['exposed_form']['type'] = 'basic';
		$handler->display->display_options['pager']['type'] = 'full';
		$handler->display->display_options['pager']['options']['items_per_page'] = '10';
		$handler->display->display_options['style_plugin'] = 'table';
		/* No results behavior: Global: Text area */
		$handler->display->display_options['empty']['area']['id'] = 'area';
		$handler->display->display_options['empty']['area']['table'] = 'views';
		$handler->display->display_options['empty']['area']['field'] = 'area';
		$handler->display->display_options['empty']['area']['empty'] = TRUE;
		$handler->display->display_options['empty']['area']['content'] = '暂无数据';
		$handler->display->display_options['empty']['area']['format'] = 'filtered_html';
		/* Relationship: Entity Reference: Referenced Entity */
		$handler->display->display_options['relationships']['field_uuke_alipay_product_nid_target_id']['id'] = 'field_uuke_alipay_product_nid_target_id';
		$handler->display->display_options['relationships']['field_uuke_alipay_product_nid_target_id']['table'] = 'field_data_field_uuke_alipay_product_nid';
		$handler->display->display_options['relationships']['field_uuke_alipay_product_nid_target_id']['field'] = 'field_uuke_alipay_product_nid_target_id';
		$handler->display->display_options['relationships']['field_uuke_alipay_product_nid_target_id']['required'] = TRUE;
		/* Relationship: Content: Author */
		$handler->display->display_options['relationships']['uid']['id'] = 'uid';
		$handler->display->display_options['relationships']['uid']['table'] = 'node';
		$handler->display->display_options['relationships']['uid']['field'] = 'uid';
		/* Field: Content: Title */
		$handler->display->display_options['fields']['title']['id'] = 'title';
		$handler->display->display_options['fields']['title']['table'] = 'node';
		$handler->display->display_options['fields']['title']['field'] = 'title';
		$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
		$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
		/* Field: Content: Icon */
		$handler->display->display_options['fields']['field_app_icon']['id'] = 'field_app_icon';
		$handler->display->display_options['fields']['field_app_icon']['table'] = 'field_data_field_app_icon';
		$handler->display->display_options['fields']['field_app_icon']['field'] = 'field_app_icon';
		$handler->display->display_options['fields']['field_app_icon']['relationship'] = 'field_uuke_alipay_product_nid_target_id';
		$handler->display->display_options['fields']['field_app_icon']['click_sort_column'] = 'fid';
		$handler->display->display_options['fields']['field_app_icon']['settings'] = array(
		  'image_style' => 'sc50',
		  'image_link' => 'content',
		);
		/* Field: Content: Product Nid */
		$handler->display->display_options['fields']['field_uuke_alipay_product_nid']['id'] = 'field_uuke_alipay_product_nid';
		$handler->display->display_options['fields']['field_uuke_alipay_product_nid']['table'] = 'field_data_field_uuke_alipay_product_nid';
		$handler->display->display_options['fields']['field_uuke_alipay_product_nid']['field'] = 'field_uuke_alipay_product_nid';
		$handler->display->display_options['fields']['field_uuke_alipay_product_nid']['settings'] = array(
		  'link' => 0,
		);
		/* Field: Content: 商品价格 */
		$handler->display->display_options['fields']['field_uuke_alipay_product_price']['id'] = 'field_uuke_alipay_product_price';
		$handler->display->display_options['fields']['field_uuke_alipay_product_price']['table'] = 'field_data_field_uuke_alipay_product_price';
		$handler->display->display_options['fields']['field_uuke_alipay_product_price']['field'] = 'field_uuke_alipay_product_price';
		$handler->display->display_options['fields']['field_uuke_alipay_product_price']['settings'] = array(
		  'thousand_separator' => ' ',
		  'decimal_separator' => '.',
		  'scale' => '2',
		  'prefix_suffix' => 1,
		);
		/* Field: Content: Updated date */
		$handler->display->display_options['fields']['changed']['id'] = 'changed';
		$handler->display->display_options['fields']['changed']['table'] = 'node';
		$handler->display->display_options['fields']['changed']['field'] = 'changed';
		$handler->display->display_options['fields']['changed']['date_format'] = 'custom';
		$handler->display->display_options['fields']['changed']['custom_date_format'] = 'Y-m-d h:i:s';
		/* Field: Content: 订单状态 */
		$handler->display->display_options['fields']['field_uuke_alipay_trade_status']['id'] = 'field_uuke_alipay_trade_status';
		$handler->display->display_options['fields']['field_uuke_alipay_trade_status']['table'] = 'field_data_field_uuke_alipay_trade_status';
		$handler->display->display_options['fields']['field_uuke_alipay_trade_status']['field'] = 'field_uuke_alipay_trade_status';
		/* Field: Content: Pay link */
		$handler->display->display_options['fields']['field_uuke_alipay_link']['id'] = 'field_uuke_alipay_link';
		$handler->display->display_options['fields']['field_uuke_alipay_link']['table'] = 'field_data_field_uuke_alipay_link';
		$handler->display->display_options['fields']['field_uuke_alipay_link']['field'] = 'field_uuke_alipay_link';
		$handler->display->display_options['fields']['field_uuke_alipay_link']['exclude'] = TRUE;
		/* Field: Global: Custom text */
		$handler->display->display_options['fields']['nothing']['id'] = 'nothing';
		$handler->display->display_options['fields']['nothing']['table'] = 'views';
		$handler->display->display_options['fields']['nothing']['field'] = 'nothing';
		$handler->display->display_options['fields']['nothing']['label'] = '订单操作';
		$handler->display->display_options['fields']['nothing']['alter']['text'] = '付款链接';
		$handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
		$handler->display->display_options['fields']['nothing']['alter']['path'] = '[field_uuke_alipay_link]';
		$handler->display->display_options['fields']['nothing']['alter']['external'] = TRUE;
		/* Sort criterion: Content: Post date */
		$handler->display->display_options['sorts']['created']['id'] = 'created';
		$handler->display->display_options['sorts']['created']['table'] = 'node';
		$handler->display->display_options['sorts']['created']['field'] = 'created';
		$handler->display->display_options['sorts']['created']['order'] = 'DESC';
		/* Filter criterion: Content: Published */
		$handler->display->display_options['filters']['status']['id'] = 'status';
		$handler->display->display_options['filters']['status']['table'] = 'node';
		$handler->display->display_options['filters']['status']['field'] = 'status';
		$handler->display->display_options['filters']['status']['value'] = 1;
		$handler->display->display_options['filters']['status']['group'] = 1;
		$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
		/* Filter criterion: Content: Type */
		$handler->display->display_options['filters']['type']['id'] = 'type';
		$handler->display->display_options['filters']['type']['table'] = 'node';
		$handler->display->display_options['filters']['type']['field'] = 'type';
		$handler->display->display_options['filters']['type']['value'] = array(
		  'uuke_alipay_order' => 'uuke_alipay_order',
		);

		/* Display: All order list page */
		$handler = $view->new_display('page', 'All order list page', 'all_order_list');
		$handler->display->display_options['path'] = 'order-list';
		$handler->display->display_options['menu']['type'] = 'normal';
		$handler->display->display_options['menu']['title'] = 'All Order LIst';
		$handler->display->display_options['menu']['weight'] = '0';
		$handler->display->display_options['menu']['context'] = 0;

		/* Display: My order list page */
		$handler = $view->new_display('page', 'My order list page', 'my_order_list');
		$handler->display->display_options['defaults']['title'] = FALSE;
		$handler->display->display_options['title'] = 'My Order LIst';
		$handler->display->display_options['defaults']['filter_groups'] = FALSE;
		$handler->display->display_options['defaults']['filters'] = FALSE;
		/* Filter criterion: Content: Published */
		$handler->display->display_options['filters']['status']['id'] = 'status';
		$handler->display->display_options['filters']['status']['table'] = 'node';
		$handler->display->display_options['filters']['status']['field'] = 'status';
		$handler->display->display_options['filters']['status']['value'] = 1;
		$handler->display->display_options['filters']['status']['group'] = 1;
		$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
		/* Filter criterion: Content: Type */
		$handler->display->display_options['filters']['type']['id'] = 'type';
		$handler->display->display_options['filters']['type']['table'] = 'node';
		$handler->display->display_options['filters']['type']['field'] = 'type';
		$handler->display->display_options['filters']['type']['value'] = array(
		  'uuke_alipay_order' => 'uuke_alipay_order',
		);
		/* Filter criterion: User: Current */
		$handler->display->display_options['filters']['uid_current']['id'] = 'uid_current';
		$handler->display->display_options['filters']['uid_current']['table'] = 'users';
		$handler->display->display_options['filters']['uid_current']['field'] = 'uid_current';
		$handler->display->display_options['filters']['uid_current']['relationship'] = 'uid';
		$handler->display->display_options['filters']['uid_current']['value'] = '1';
		$handler->display->display_options['path'] = 'user/%user/order-list';
		$handler->display->display_options['menu']['type'] = 'tab';
		$handler->display->display_options['menu']['title'] = 'My Order LIst';
		$handler->display->display_options['menu']['weight'] = '0';
		$handler->display->display_options['menu']['name'] = 'user-menu';
		$handler->display->display_options['menu']['context'] = 0;
	}
	$views[$view->name] = $view;
	// return views
	return $views;
}

	
