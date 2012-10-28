// $Id: scribeseo.js,v 1.1.2.8 2010/09/26 00:43:12 tomdude48 Exp $
if (Drupal.jsEnabled) {
  $(document).ready(function () {
	  if(Drupal.settings.scribeseo.get_form_elements == 1) {
	    //scribeseo_get_form_elements();	 
    }
	  if(Drupal.settings.scribeseo.node_admin_content_form == 1) {
	    node_admin_content_form();	 
    }
  });
}

var scribeseo_init_report = function() {
  $('a.scribe-content-keyword').click( function(event) {
    scribeseo_click_content_keyword(this, event);
  });
}

// Implementation of hook_contentanalysis_data
var scribeseo_contentanalysis_data = function(aid) {		
  data = new Array();  
  data['usecache'] = scribeseo_usecache_input();  
  return data;
}

var scribeseo_usecache_input = function() {
  r = 0;  
  if($('#edit-scribeseo-usecache-1:checked').val() == '1') { 
    r = 1;  
  }	
  // Drupal formats id differently based if embed in node edit or via ajax
  if($('#edit-scribeseo-scribeseo-usecache-1:checked').val() == '1') { 
    r = 1;  
  }	
  return r;
}

// Implementation of hook_contentanalysis_analysis_success
var scribeseo_contentanalysis_analysis_success = function(aid) {
  scribeseo_init_report();	
  scribeseo_get_form_elements();	
}

var scribeseo_get_form_elements = function() {
  // if TinyMCE is used, turn off and on to save body text to textarea
  var data = { 
    'nid': Drupal.settings.scribeseo.nid,
    'path': Drupal.settings.scribeseo.path,
    'mode': Drupal.settings.scribeseo.mode,
    'usecachedefault': 0
  };
  data['usecachedefault'] = scribeseo_usecache_input();  
//return;  

  $.ajax({
    type: 'POST',
    url: Drupal.settings.scribeseo.get_form_elements_callback,
    data: data,
    dataType: 'json',
    success: function(data, textStatus) {
      $('#scribeseo_form_elements').replaceWith(data.main['output']);
      //alert(data.main['output']);
    },
    error: function(XMLHttpRequest, textStatus, errorThrown) {
      //alert("error " + XMLHttpRequest.toString());
    }
  });
  return false;	
}


var scribeseo_click_content_keyword = function(theLink, event) {
  event.preventDefault();

  var jthis = $(theLink);
 
  if(jthis.hasClass('active')) {
    return;
  }
  var previous = $('#scribe-content-keywords a.active');
  
  previous.removeClass('active');
  jthis.addClass('active');
  
  var id = jthis.attr('id');
  id = id.replace("scribe-content-keyword-tab-", "scribe-content-keyword-table-");

  toRemove = $('#scribe-alternate-keywords .alternate-keywords-table').hide(); 
  
  var existing = $('#' + id);
  if(existing.size() > 0) {
    $(existing).show();
  } else {
    $('#fetching-alternate-keywords-message').show();
    var data = { 
      'action': 'ecordia_keyword_alternates',
      'keyword': jthis.text()
    };
    $.ajax({
      type: 'POST',
      url: Drupal.settings.scribeseo.alternate_keywords_callback,
      data: data,
      dataType: 'json',
      success: function(data, textStatus) {
        $('#fetching-alternate-keywords-message').hide();
        $('#scribe-alternate-keywords').append(data.output);
        //$('.alternate-keywords-table').tablesorter({
        //	textExtraction: 'complex'
        //});
        if(typeof kwresearch_process_keywords == 'function') {
          kwresearch_process_keywords();
        }
      },
      error: function(XMLHttpRequest, textStatus, errorThrown) {
        //alert("error " + XMLHttpRequest.toString());
      }
    });
  }
}


var node_admin_content_form = function() {	
  $('thead tr').find('th:last').before(Drupal.settings.scribeseo.th);  
  nodes = Drupal.settings.scribeseo.nodes;
  for ( var i in nodes ) {
    $('tbody tr:has(#edit-nodes-' + i + ')').find('td:last').before(nodes[i]);
  }
}