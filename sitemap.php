<?php

	require_once("sites/default/settings.php");

	// connect to database
	function connect($databases)
	{
		$db_host = $databases["default"]["default"]["host"];
		$db_user = $databases["default"]["default"]["username"];
		$db_pass = $databases["default"]["default"]["password"];
		$db_name = $databases["default"]["default"]["database"];

		if(!$link = mysql_connect($db_host, $db_user, $db_pass))
		{
			$result = 0;
			print("Error connecting to MySQL Server [$db_host] with user account [$db_user]!<br>\n");
		}
		else
		{
			if(!$conn = mysql_select_db($db_name,$link))
			{
				print("error selecting database<br>\n");
			}
		}
	}

	connect($databases);
	
	$select = "select * from url_alias";
	$res = mysql_query($select);
	while($row = mysql_fetch_object($res))
	{
		$urls[substr($row->source,5)] = $row->alias;
	}
		
	
	$select = "select * from field_data_field_list_title";
	$res = mysql_query($select);
	while($row = mysql_fetch_object($res))
	{
		$all_guides[$row->entity_id] = $row;
	}

	
	$select = "select * from field_data_field_guide_category";
	$res = mysql_query($select);
	while($row = mysql_fetch_object($res))
	{
		$all_child_guides[$row->entity_id][$row->field_guide_category_target_id] = $row;
		$all_links[$row->field_guide_category_target_id][$row->entity_id] = $row;
	}

	$select = "select * from draggableviews_structure";
	$res = mysql_query($select);
	while($row = mysql_fetch_object($res))
	{
		$ordering[substr($row->args,2,-2)][$row->entity_id] = $row->weight;
	}
	foreach($ordering as $entity_id => $orders)
	{
		asort($ordering[$entity_id]);
	}

	
?>
<title>DIY Table of Contents</title>
<style>
ul {list-style-type:none;}
div#breadcrumbs {margin: 0;text-align: left;background-color: white;color: black;}
.breadcrumbs {font-size: 90%;line-height: 100%;}
div#breadcrumbs div.breadcrumb {padding: 5px 0 8px 0}
.level1 {font-weight:bold; font-size:16px;}
.level2 {font-size:15px;}
.level3 {font-size:14px;}
.level4 {font-size:13px;}
.level5 {font-size:12px;}
.level6 {font-size:11px;}
.level7 {font-size:10px;}
</style>

<div id="PSUContent">

			  
<h1 style="margin-bottom:0">DIY Table of Contents</h1>
<div id="breadcrumbs" class="breadcrumbs block">
<div id="breadcrumbs-inner" class="breadcrumbs-inner gutter">
  <div class="breadcrumb"><a href="index.php">Library DIY</a> &raquo; DIY Table of Contents</div>
</div><!-- /breadcrumbs-inner -->
</div>
<?php
	
	print("<hr><ul>\n");
	
	$entity_id = 0;
	$level = 0;
	$section_ordering = array();
	asort($ordering[$entity_id]);
	foreach($ordering[$entity_id] as $child_entity_id => $order)
	{
		if(isset($all_links[$child_entity_id]))
		{
			print_child_guide($child_entity_id,false,($level+1));
		}
	}
	print("</ul><hr>\n");
	
	$entity_id = 0;
	$level = 0;
	$section_ordering = array();
	asort($ordering[$entity_id]);
	print("<ul>\n");
	foreach($ordering[$entity_id] as $child_entity_id => $order)
	{
		if(isset($all_links[$child_entity_id]))
		{
			print_child_guide($child_entity_id,true,($level+1));
		}
	}
	print("</ul>\n");
	
	
	function print_child_guide($entity_id,$recursive,$level)
	{
		global $urls, $all_links, $all_guides, $ordering, $count;
		$guide = $all_guides[$entity_id];
		
		if($level < 3 || strcmp(strip_tags($guide->field_list_title_value),"These do not describe my need"))
		{
			if($recursive)
			{
				$name = "guide".$entity_id;
				$link = $urls[$entity_id];
				$label = "<div class='level$level'><img src='sites/all/themes/diy/i/dot.gif' style='padding-right:5px'>".strip_tags($guide->field_list_title_value)."</div>";
			}
			else
			{
				$name = "";
				$link = "#guide$entity_id";
				$label = "<div class='level$level'><img src='sites/all/themes/diy/i/down-arrow.gif'>".strip_tags($guide->field_list_title_value)."</div>";
			}
			print("<li><a name='$name' href='$link'>");
			print("$label");
			print("</a></li>\n");
			if($recursive && isset($ordering[$entity_id]) && isset($all_links[$entity_id]))
			{
			// print_r($ordering[$entity_id]);
			// print_r($all_links[$entity_id]);
			// exit();
				print("<ul>\n");
				$section_ordering = array();
				foreach($all_links[$entity_id] as $child_entity_id => $obj)
				{
					$section_ordering[$obj->entity_id] = $ordering[$entity_id][$obj->entity_id];
				}
				// print_r($section_ordering);
				asort($section_ordering);
				foreach($section_ordering as $child_entity_id => $order)
				{
					// print("order: $entity_id:<br>\n");
					// print_r($child_entity_id);
					// $count++;
					// if($count > 20) exit();
					print_child_guide($child_entity_id,$recursive,($level+1));
				}
				print("</ul>\n");
			}
		}
	}
?>
</div>
