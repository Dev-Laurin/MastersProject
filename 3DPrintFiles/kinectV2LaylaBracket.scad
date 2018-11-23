//kinectV2LaylaBracket.scad
//10-27-18

//milimeters = units 
//25cm long, 4.1cm high, 6.7cm deep 
kinectWidth = 250;  
kinectHeight = 41; 
kinectDepth = 67; 

difference(){
    //Outward layer
    //add cm thickness 
    width = 10; 
    bottomLayer = 4; 
    
    union(){
        translate([0, -width/2, 0]) 
            cube([kinectWidth + width, kinectDepth + width, kinectHeight + width]);
        
        //give more layers to backing on top for support 
        translate([0, -width/2,kinectHeight/2 ]) 
            cube([kinectWidth + width, 40,  kinectHeight - width/2]);
    }
    
    //Inside (kinect) 
    translate([-kinectWidth/2, 0, width/2 - (width/2 - bottomLayer)]) 
        cube([kinectWidth *2, kinectDepth,  kinectHeight]); //width, height, depth ? But wid,hei looks swapped   
    
    //take out walls
    translate([width,-kinectHeight/2,-kinectDepth/2 ]) 
        cube([kinectWidth - width, kinectDepth *2,  kinectHeight*2]);
   
    //take out front part so we have a connector on the top back 
    translate([width,35,-kinectDepth/2 +10 ]) 
        cube([kinectWidth - width, kinectDepth *2,  kinectHeight*2]);    
    
    //split the halves 
    translate([kinectWidth/2,-kinectDepth/2,-kinectDepth/2 +10 ]) 
        cube([width*2, kinectDepth *2,  kinectHeight*2]);   
    
     
}
width = 10; 

difference(){
 //add connectors for halves
    translate([kinectWidth/2 + width*2,-width/2,kinectHeight + width -2 ]) 
        cube([width/2, 40 ,  width*2]);  

screwDiameter = 2.5; 
translate([kinectWidth/2 +width,width,kinectHeight + width*2 + 2 ]) 
    rotate([0, 90, 0])
        cylinder(width*4, screwDiameter, screwDiameter, true); 
}

difference(){
    translate([kinectWidth/2 -width/2,-width/2,kinectHeight + width -2 ]) 
        cube([width/2, 40 ,  width*2]); 
    
    screwDiameter = 2.5; 
translate([kinectWidth/2 +width,width,kinectHeight + width*2 + 2 ]) 
    rotate([0, 90, 0])
        cylinder(width*4, screwDiameter, screwDiameter, true); 
}

//The top latches



difference(){
union(){
translate([kinectWidth/2 +width*6,0,kinectHeight + width ]) 
    cube([40, 30, 80]); 

translate([kinectWidth/2 -width*8,0,kinectHeight + width ]) 
    cube([40, 30, 80]); 
translate([kinectWidth/2 -width*8,0,kinectHeight + width + 80 ]) 
    cube([40, 80, 10]); 

translate([kinectWidth/2 +width*6,0,kinectHeight + width + 80 ]) 
    cube([40, 80, 10]); 
}
//bevel
/*
translate([50 + kinectWidth/2 + width*2, width*4 + -5, width*12 + 4])
    rotate([0, 90, 0])
        cylinder(50, 10, 10, true); 
*/ 

//screw holes on top 
translate([kinectWidth/2 +width*8,kinectDepth/2 + width*2,kinectHeight + width + 80 ]) 
    cylinder(40, 3, 3, true); 

//screw holes on top 
translate([kinectWidth/2 -width*6,kinectDepth/2 + width*2,kinectHeight + width + 80 ]) 
    cylinder(40, 3, 3, true); 
}

