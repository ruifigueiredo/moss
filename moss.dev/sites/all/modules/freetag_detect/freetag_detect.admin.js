(function ($) {

    Drupal.behaviors.freetagDetect = {
        attach:function (context, settings) {
            var label;
            var clickedId;
            var wysiwygWasDisabled = false;
            // fix default behaviour
            $(".freetag-detect-scan-button").click(function (e) {
                e.preventDefault();
                return false;
            });

            $('.freetag-detect-scan-button').bind('click', function () {
                label = $(this).attr('value');
                clickedId = $(this).attr('id');

                $(this).attr('value', 'Scanning...');
                $(this).attr('disabled', 'disabled');
                // disable wysiwyg body if its available
                if(Drupal.settings.wysiwyg) {
                    // use the first one, this how a few modules do it
                    var item = $('.wysiwyg')[0];
                    var params = Drupal.settings.wysiwyg.triggers[item.id];
                    Drupal.wysiwygDetach(this, params['format'+$(item).val()]);
                    wysiwygWasDisabled  = true;
                } else {
                    wysiwygWasDisabled = false;
                }

                // depending on the ID that called, which field are we configured to scan?
                var key = clickedId.replace('-scan-button','');
                var source_field = '#edit-'+Drupal.settings.freetag_detect[key].source+'-und-0-value';
                var content = $(source_field).attr('value');

                if(wysiwygWasDisabled) {
                    Drupal.wysiwygAttach(this, params['format'+$(item).val()]);
                }


                // post our content to our callback which will return keywords to mangle into the auto tag field
                jQuery.ajax({
                    type:'POST',
                    url:'/freetag_detect/getKeywords',
                    dataType:'json',
                    success:ajaxCompleted,
                    data: { content:content,
                            id:clickedId,
                            existing:$('#'+key).val(),
                            case_sensitive:Drupal.settings.freetag_detect[key].case_sensitive
                    }
                });

                /**
                 * Callback should return
                 * terms - array of terms to use
                 * fieldId - fieldId to update (the taxonomy term freetag field)
                 * @param data
                 */
                function ajaxCompleted(data) {
                    // Parse Json
                    // Add some stuff to your DOM.
                    $('#'+clickedId).removeAttr("disabled");
                    $('#'+clickedId).attr('value', label);

                    try {
                        $('#' + data.fieldId).attr('value', data.terms.join(','));
                    }
                    catch (err) {
                        alert('Bad response from freetagging autodection');
                    }


                }
            });
        }
    };

})(jQuery);

