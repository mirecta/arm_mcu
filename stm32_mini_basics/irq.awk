#!/usr/bin/gawk -f
BEGIN{

i = 0;
}

{

    irq[i] = $2;
    ++i;
}
END{
for (idx in irq){
    gsub(/[\n\r]/,"",irq[idx]);
    
    if (idx > 0 && irq[idx] != 0){
    print "__weak_alias_Default_Handler(",irq[idx],");";
}
    }

    print "\n\n\n";

    for (idx in irq){
       
        print " ",irq[idx], ", \t\t // " idx - 15; 
   
    }

}
