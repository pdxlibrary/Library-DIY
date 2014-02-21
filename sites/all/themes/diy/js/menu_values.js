			
			if(typeof(base_site_url) == "undefined")
				base_site_url = "http://library.pdx.edu";
			
			(function () {
			
				var Event = YAHOO.util.Event,
					Dom = YAHOO.util.Dom,
					UA = YAHOO.env.ua,

					/*
						 Define an array of object literals, each containing 
						 the data necessary to create the items for a MenuBar.
					*/
									
					aItemData = [
		
						{ text: "Library Home", url: base_site_url + "/"},
						
						{ text: "Resources", url: base_site_url + "/findit.html", 
						
							submenu: { id: "resources_menu", itemdata: [
									{ text: "PSU-Only Catalog (Vikat)", url: "http://vikat.pdx.edu"},
									{ text: "Portland State WorldCat", url: "http://portlandstate.worldcat.org/advancedsearch"},
									{ text: "Databases &amp; Articles", url: base_site_url + "/dofd/" },
									{ text: "Find Journals", url: "http://wq5rp2ll8a.search.serialssolutions.com/" },
									{ text: "Government Information", url: base_site_url + "/governmentinformationservice.html" },
									{ text: "Special Collections", url: base_site_url + "/specialcollections.html" },
									{ text: "Reference Sources", url: "http://www.lib.pdx.edu/guides/resources.php?category=81&item_id=1887" },
									{ text: "More ...", url: base_site_url + "/findit.html" }
							] }
				
						},
						
						{ text: "Research Help", url: base_site_url + "/researchassistance.html",
							submenu: { id: "research_help_menu", itemdata: [
									{ text: "Ask Us!", url: base_site_url + "/askus.html" },
		                            { text: "Research Guides &amp; Tutorials", url: base_site_url + "/researchguides.html" },
		                            { text: "Citing Sources", url: base_site_url + "/citing_sources.html" },
									{ text: "Workshops &amp; Tours", url: base_site_url + "/workshops_and_tours.html" },
									{ text: "More ...", url: base_site_url + "/researchassistance.html" }
							] }
						},
						{ text: "Services", url: base_site_url + "/services.html",
							submenu: { id: "services_menu", itemdata: [
	                            {
	                                text: "For You", 
	                                submenu: { 
	                                            id: "services_for_you", 
	                                            itemdata: [
	                                                { text: "Faculty", url: base_site_url + "/faculty_services.html" },
	                                                { text: "Students", url: base_site_url + "/studentlibraryservices.html" },
	                                                { text: "Distance Users", url: base_site_url + "/distancestudentservices.html" }
	                                            ] 
	                                        }
	                            
	                            },
								{ text: "Borrowing", url: base_site_url + "/checkout.html" },
	                            { text: "Renew Online", url: base_site_url + "/MyAccounts" },
	                            { text: "Interlibrary Loan - ILLiad", url: "https://ill.lib.pdx.edu/Logon.php" },
	                            { text: "Instructional Services", url: base_site_url + "/instructionservices.html" },
	                            { text: "Technology in the Library", url: base_site_url + "/technology-in-the-library.html" },
	                            { text: "Print, Scan, Copy", url: base_site_url + "/print-scan-copy.html" },
	                            { text: "Group Study Rooms", url: base_site_url + "/groupstudy.html" },
								{ text: "More ...", url: base_site_url + "/services.html" }      
							] }
						},
						{ text: "About PSU Library", url: base_site_url + "/about.html",
							submenu: { id: "about_menu", itemdata: [
	                            { text: "Contact Us", url: base_site_url + "/staff_directory" },
	                            { text: "Maps &amp; Directions", url: base_site_url + "/maps.html" },
	                            { text: "Jobs", url: base_site_url + "/jobs.html" },
	                            { text: "Hours", url: base_site_url + "/hours.html" },
	                            { text: "Library News", url: base_site_url + "/blogs" },
	                            { text: "Support the Library", url: base_site_url + "/support.html" },
								{ text: "More ...", url: base_site_url + "/about.html" }     
							] }
						}
						
				
					];


				/*
					 Initialize and render the MenuBar when the page's DOM is ready 
					 to be scripted.
				*/
	
				Event.onDOMReady(function () {
	
					/*
						Instantiate a MenuBar:  The first argument passed to the constructor
						is the id for the Menu element to be created, the second is an 
						object literal of configuration properties.
					*/
				
					var oMenuBar = new YAHOO.widget.MenuBar("PSU_Menubar", 
						{ 
							lazyload: false,
							autosubmenudisplay: true,
							showdelay: 0,
							hidedelay: 500
						}
					);
	
					var onMenuItemClick = function (p_sType, p_aArgs) {
					    var oEvent = p_aArgs[0],    //  DOM event
							oMenuItem = p_aArgs[1]; //  MenuItem instance that was the target of the event   
  
						if (oMenuItem)
						{
							// ignore local anchor links
							if(oMenuItem.cfg.getProperty("url").substr(0,1) != "#")
								document.location.href = oMenuItem.cfg.getProperty("url");
						}

						//alert("Callback for MenuItem: " + this.cfg.getProperty("id"));
					};
					
					function onSubmenuShow() {
						var oIFrame;

						/*
							Need to set the width for submenus of submenus in IE to prevent the mouseout 
							event from firing prematurely when the user mouses off of a MenuItem's 
							text node.
						*/
	
						//alert("testing submenu loading..."+this.id);
						if ((this.id == "resources_menu" || this.id == "research_help_menu" || this.id == "reference_sources" || this.id == "services_menu" || this.id == "services_for_you" || this.id == "about_menu") && YAHOO.env.ua.ie) {
	
							oElement = this.element;
							nOffsetWidth = oElement.offsetWidth;
					
							/*
								Measuring the difference of the offsetWidth before and after
								setting the "width" style attribute allows us to compute the 
								about of padding and borders applied to the element, which in 
								turn allows us to set the "width" property correctly.
							*/
							
							oElement.style.width = nOffsetWidth + "px";
							oElement.style.width = (nOffsetWidth - (oElement.offsetWidth - nOffsetWidth)) + "px";
						
						}
					
					}
					
					oMenuBar.addItems(aItemData);
					// Subscribe to the "show" event for each submenu
					oMenuBar.subscribe("show", onSubmenuShow);
					oMenuBar.subscribe("click", onMenuItemClick);
					oMenuBar.render("PSULibrary_Primary_Navbar");
					
				});
			
			}());
		
