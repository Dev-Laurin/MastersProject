//groundPlaneDisplay.js
//Laurin Fisher
//9-6-18
//To draw the ground planes produced by RANSAC with Kinect v2 data. 

//GLOBALS 
//Set the scene size. 
const WIDTH = 1000; 
const HEIGHT = 800; 

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
var geometry = new THREE.PlaneGeometry(grid.length *2 , grid.length *2,
 32); 
var material = new THREE.MeshBasicMaterial( {
	color: 0xffff00, side: THREE.DoubleSide
}); 
var plane = new THREE.Plane(new THREE.Vector3(1, 1, 0.2), 3);
var helper = new THREE.PlaneHelper(plane, 100, 0xffff00); 
//scene.add(helper);   

var i = 0; 
var frame = 0; 

//2D Ground Grid 
var groundGridSize = 200; 
var groundDivisions = grid.length * 2; 

var controls = new THREE.OrbitControls(camera); 


//Add Kinect for reference size 
var geometry = new THREE.BoxGeometry(25, 
					6, 6);
var material = new THREE.MeshBasicMaterial({color: 0xD3D3D3}); 
var kinect = new THREE.Mesh(geometry, material);
scene.add(kinect); 

init();

function init(){

	//Add the camera to the scene.
	scene.add(camera); 
	//scene.add(sphere); 

	//Start the renderer. 
	renderer.setSize(WIDTH, HEIGHT); 

	//Attach the renderer-supplied DOM element. 
	container.appendChild(renderer.domElement); 

//Preferred Camera position

	camera.position.z = -180;
	camera.position.y = 170; 
	camera.rotation.x = 0; 

	controls.update(); 

	//create a point light 
	const pointLight = new THREE.PointLight(0xFFFFFF, 1, 100); 
	pointLight.castShadow = true; 

	//set its position 
	pointLight.position.x = 10; 
	pointLight.position.y = 50; 
	pointLight.position.z = 130; 

	//add to the scene
	scene.add(pointLight); 

	drawBins(); 

	//schedule the first frame 
	requestAnimationFrame(update); 

} 

function drawBins(){
	//grid is our variable with our data 

	var groundSquareSize = binSize * 100; // groundGridSize/groundDivisions - 0.5; //account for lines 
	console.log(groundSquareSize)

	for(var x=0; x<grid.length; x++){
		//The X bin position from left to right in view range 
		var realXBinCoord = x - (grid.length/2); 

		if(realXBinCoord < 0){
			realXBinCoord = realXBinCoord * groundSquareSize + 
					groundSquareSize/2;
		}
		else{
			realXBinCoord = realXBinCoord * groundSquareSize - 
					groundSquareSize/2; 
		}

		for(var z=0; z<grid[x].length; z++){
			//The Z bin from 0 forward 

			if(grid[x][z].y != 0){
				//Cube model 
				var geometry = new THREE.BoxGeometry(groundSquareSize, 
					grid[x][z].y * 100 /*groundSquareSize */ , groundSquareSize);
				
				//if the object is greater than 5 cm - not drivable 
				var color = 0xff0000; 
				if(grid[x][z].y * 100 > cmCanDrive){
					color = 0xff0000; 
				}
				else{
					color = 0x00ff00; 
				}

				var material = new THREE.MeshBasicMaterial({color: color}); 
				var cube = new THREE.Mesh(geometry, material);
 
				//Wireframe of cube  
				var wireframe = new THREE.WireframeHelper(cube, 0x000000); 

				//Position of cube 

				cube.position.x = realXBinCoord; 
				cube.position.y += (grid[x][z].y *100)/2; // (grid[x][z].y * groundSquareSize)/2; 
				cube.position.z =  z * (groundSquareSize) + groundSquareSize/2; 

				//Position of wireframe 
				wireframe.position.x = realXBinCoord; 
				wireframe.position.y += (grid[x][z].y *100)/2;// (grid[x][z].y * groundSquareSize)/2; 
				wireframe.position.z =  z * (groundSquareSize) + groundSquareSize/2; 

				scene.add(cube); 
				scene.add(wireframe); 
			}
		}
		
	}
	
}

//Rotating camera functionality
//https://stackoverflow.com/questions/8426822/rotate-camera-in-three-js-with-mouse
function update() {
	reloadData(); 

	controls.update(); 
	 
	//Draw
	renderer.render(scene, camera); 

	//schedule the next frame. 
	requestAnimationFrame(update); 
	frame++; 
}

//reload variable js file 
function reloadData(){
	var head = document.getElementsByTagName('head')[0]; 
	var script = document.createElement('script'); 
	script.src = 'data.js'; 
	head.appendChild(script); 
}