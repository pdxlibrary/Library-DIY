		$(document).ready(function() {
		
			//Default Action
			$(".tab_content").hide(); //Hide all content
			$("ul.tabs li:first").addClass("active").show(); //Activate first tab
			$(".tab_content:first").show(); //Show first tab content
			
			// set the initial text note in the search box
			var activeSearchBox = $("div.tab_content div form input.easyInput");
			$(activeSearchBox).attr("value","Find books and more with Portland State WorldCat ...");
			
			//On Click Event | Search Tab Clicked
			$("ul.tabs li").click(function() {
				// check to see if user has already entered something
				var prevActiveTab = $("ul.tabs li.active");
				var prevActiveTabID = $(prevActiveTab).attr("id");
				var entered = "";
				if(prevActiveTabID == "st_all")
					entered = $("div.tab_content#all div form input.easyInput").val();
				else if(prevActiveTabID == "st_books")
					entered = $("div.tab_content#books div form input.easyInput").val();
				else if(prevActiveTabID == "st_articles")
					entered = $("div.tab_content#articles div form input.easyInput").val();
				else if(prevActiveTabID == "st_jtitles")
					entered = $("div.tab_content#jtitles div form input.easyInput").val();	
				else if(prevActiveTabID == "st_av")
					entered = $("div.tab_content#av div form input.easyInput").val();
				
				$("ul.tabs li").removeClass("active"); //Remove any "active" class
				$(this).addClass("active"); //Add "active" class to selected tab
				$(".tab_content").hide(); //Hide all tab content
				var activeTab = $(this).find("a").attr("href"); //Find the rel attribute value to identify the active tab + content
				//alert("atab: " + activeTab);
				$(activeTab).show();
				
				// set the text note in the search box
				var activeSearchBox = $("div.tab_content div form input.easyInput");
				if(entered!="" && entered!="Find books and more with Portland State WorldCat ..." && entered!="Find books ..." && entered!="Find articles from selected databases ..." && entered!="Find journals by title, for example \"New York Times\" ..."  && entered!="Find DVDs, CDs, streaming media, and more ...")
				{
					$(activeSearchBox).attr("value",entered);
				}
				else
				{
					if(activeTab == "#all")
						$(activeSearchBox).attr("value","Find books and more with Portland State WorldCat ...");
					else if(activeTab == "#books")
						$(activeSearchBox).attr("value","Find books ...");
					else if(activeTab == "#articles")
						$(activeSearchBox).attr("value","Find articles from selected databases ...");
					else if(activeTab == "#jtitles")
						$(activeSearchBox).attr("value","Find journals by title, for example \"New York Times\" ...");
					else if(activeTab == "#av")
						$(activeSearchBox).attr("value","Find DVDs, CDs, streaming media, and more ...");
				}
				return false;
			});
			
			//On Click Event | Clear the textbox when selected
			$("div.tab_content div form input.easyInput").click(function() {
				if($(this).val()=="Find books and more with Portland State WorldCat ..." || $(this).val()=="Find books ..." || $(this).val()=="Find articles from selected databases ..." || $(this).val()=="Find journals by title, for example \"New York Times\" ..."  || $(this).val()=="Find DVDs, CDs, streaming media, and more ...")
					$(this).attr('value', '');
					
				return false;
			});
		
		});