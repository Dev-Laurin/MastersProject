//Author: Laurin Fisher
//Date Revised: 
//Purpose: To transfer data from C++ to Javascript for my Master's Project in Kinect Object Detection.

//Our point data in bins. X bins, then inside those are Z bins.
var grid = [ [ {x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
 ], 
 [ {x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
 ], 
 [ {x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: -2, y: 3, z: 3 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
 ], 
 [ {x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
 ], 
 [ {x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
 ], 
 [ {x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
 ], 
 [ {x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
 ], 
 [ {x: 0, y: 0, z: 0 }, 
{x: 1, y: 1, z: 1 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
 ], 
 [ {x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
 ], 
 [ {x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
{x: 0, y: 0, z: 0 }, 
 ], 
 ]; 

//The width of our kinect view (how many squares in X direction).
var viewWidth = 10 ;
//The depth of our kinect view (how many squares in Z direction).
var viewDepth = 10 ;
