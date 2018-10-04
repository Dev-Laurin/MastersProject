# MastersProject

Why: My University requires a Masters Project with a corresponding paper in order for me to get my Masters Degree in CS. And
  I like robots. Robot vision has interested me from day one.
  
About: Real time object detection using a Kinect v2 on a Layla robot controlled remotely on robotmoose.com. This project
  seeks to add to Layla's functionality by having a basic 'can't drive in this square' program, then updating Layla's view
  on the site. This will enable remote users, and possibly a future autonomous Layla, to avoid obstacles. 
  
Dependencies: The project involves a pre-existing robotic system (Hardware = Layla) and stack(robot software control) 
  = Robotmoose.com as well as libfreenect2 for the drivers of the Kinect v2.
  
Algorithm: I plan to use RANSAC, picking 3 random points from the 3D Point Cloud, to form a floor/ground plane. This
  is evaluated by how many points are in said plane (floor points > other points to work). All other points are considered
  for segmentation into objects. The segmentation process involves using depth data to calculate the distance from other 
  points to determine if they are in the same object. 
 

Citations: 
https://github.com/OpenKinect/libfreenect2 
DOI 10.5281/zenodo.50641

