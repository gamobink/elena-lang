import forms.
import extensions.

class Window
{
    object form.
    object lblClicks.
    object btmClickMe.
    
    //Store how much clicks the user doed
    object clicksCount.
    
    constructor new
    [        
        form := SDIDialog new.
        lblClicks := Label new.
        btmClickMe := Button new.
        
        clicksCount := 0.
        
        form controls; 
            append:lblClicks;
            append:btmClickMe.
    
        form 
            set caption:"Rosseta Code";
            set x:100 y:100;
            set width:160 height:80.
    
        lblClicks 
            set x:10 y:2;
            set width:160 height:20;
            set caption:"Clicks: 0".
        
        btmClickMe 
            set x:7 y:20;
            set width:140 height:30;
            set caption:"Click me";
            set onClick(:args)[ $self $onButtonClick ]
    ]
    
    $onButtonClick
    [
        clicksCount := clicksCount + 1.
        lblClicks set caption("Clicks: " + clicksCount literal).
    ]
    
    dispatch => form.
}