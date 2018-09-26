//groundPlaneDisplay.js
//Laurin Fisher
//9-6-18
//To draw the ground planes produced by RANSAC with Kinect v2 data. 

//GLOBALS 
//Set the scene size. 
const WIDTH = 400; 
const HEIGHT = 300; 

//Set some camera attributes. 
const VIEW_ANGLE = 45; 
const ASPECT = WIDTH / HEIGHT; 
const NEAR = 0.1; 
const FAR = 10000; 

//Get the DOM element to attach to 
const container = document.querySelector('#container'); 

//Create a WebGL renderer, camera, and a scene 
const renderer = new THREE.WebGLRenderer(); 
const camera = 
	new THREE.PerspectiveCamera(
		VIEW_ANGLE, 
		ASPECT, 
		NEAR, 
		FAR
	); 

const scene = new THREE.Scene(); 

//Plane 
//(width, height, width segments, height segments)
var geometry = new THREE.PlaneGeometry(10, 10, 32); 
var material = new THREE.MeshBasicMaterial( {
	color: 0xffff00, side: THREE.DoubleSide
}); 
var plane = new THREE.Plane(new THREE.Vector3(1, 1, 0.2), 3);
var helper = new THREE.PlaneHelper(plane, 100, 0xffff00); 
scene.add(helper);   

var i = 0; 
var frame = 0; 

var text = document.getElementById('text'); 

init();

function init(){
	//Add the camera to the scene.
	scene.add(camera); 

	//Start the renderer. 
	renderer.setSize(WIDTH, HEIGHT); 

	//Attach the renderer-supplied DOM element. 
	container.appendChild(renderer.domElement); 

//	scene.add(plane); 
//	plane.rotation.x = 5; 

	//add the ground plane wireframe
	/////////////////////////
	var grid = new THREE.GridHelper(200, 10); 
	grid.setColors(0xffffff, 0xffffff);
	scene.add(grid);  
	/////////////////////

	camera.position.z = 150;
	camera.position.y = 20; 
	camera.rotation.x = 0;

	//create a point light 
	const pointLight = new THREE.PointLight(0xFFFFFF); 

	//set its position 
	pointLight.position.x = 10; 
	pointLight.position.y = 50; 
	pointLight.position.z = 130; 

	//add to the scene
	scene.add(pointLight); 

	//schedule the first frame 
	requestAnimationFrame(update); 

	text.innerText = "Plane: ( , , )"
} 

function update() {
	 
	
	//every _ frame, change the plane so the user can see
	if(frame % 120 == 0){
		//change plane to new value 
		var newPlane = planes[i]
		/*
		plane.position.x = newPlane.x;  
		plane.position.y = newPlane.y; 
		plane.position.z = newPlane.z; 
		*/ 
		var vectorThree = new THREE.Vector3(newPlane.x, newPlane.y,
		newPlane.z); 
		plane.normal = vectorThree;

		//Update the text display 
		text.innerText = "Plane: (" + plane.normal.x + 
		", " + plane.normal.y + ", " + plane.normal.z + 
		")" 

		i++; 

		if(planes.length <= i){
			i = 0; 
		}
	}
	 
	//Draw
	renderer.render(scene, camera); 

	//schedule the next frame. 
	requestAnimationFrame(update); 
	frame++; 
}