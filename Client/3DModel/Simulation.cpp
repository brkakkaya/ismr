/** Example 005 User Interface

This tutorial shows how to use the built in User Interface of
the Irrlicht Engine. It will give a brief overview and show
how to create and use windows, buttons, scroll bars, static
texts, and list boxes.

As always, we include the header files, and use the irrlicht
namespaces. We also store a pointer to the Irrlicht device,
a counter variable for changing the creation position of a window,
and a pointer to a listbox.
*/
#include "Simulation.h"


using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

// Declare a structure to hold some context for the event receiver so that it
// has it available inside its OnEvent() method.
struct SAppContext
{
	IrrlichtDevice *device;
	s32				counter;
	IGUIListBox*	listbox;
};

// Define some values that we'll use to identify individual GUI controls.
enum
{
	GUI_ID_QUIT_BUTTON = 101,
	GUI_ID_GIT,
	GUI_ID_COMBO1,
	GUI_ID_COMBO2,
	GUI_ID_BASLANGICAYARLA
};



double HIZ ;
double MetreBUYUTME ;


double KAT_Y[] = {276,695,1110};

int BULUNDUGUM_KAT = 1;
int HEDEFKAT = 0;
double DEGISIM;
stringw str;
char StringDATA[BUFSIZ];


void gitButtonClicked(int clicked);
Data_t data;
Coordinate Current = BASLANGIC1;
Coordinate Next;

IGUIComboBox* box;
IGUIComboBox* box2;
IGUIStaticText* myTextBox;
IGUIEnvironment* env;

IVideoDriver* driver;
ISceneManager* smgr;
IAnimatedMesh* mesh;
IMeshSceneNode* bina;
IAnimatedMesh* meshPlayer;
IMeshSceneNode* player;
ICameraSceneNode* cam;
vector3df playerPosition;
vector3df camPosition;
vector3df camTarget;
vector3df playerRotation;
vector3df cameraRotation;

pthread_t portThread,BPlaniThread;
int runControl = 1;
int BPlaniPanik = 0;

double XPos_Positive = 0;
double XPos_Negative = 0;
double ZPos_Positive = 0;
double ZPos_Negative = 0;
int PORT;
char* IP;

FRONT robotFront = XPOS;

void rotation(FRONT on, FRONT yeniOn, vector3df hedefPosition);
void *dataControlAndProccess(void* msg);
void GUIAdd();
void sendDataT(char* ip,int port,Data_t data);
void sendDataT(char* ip,int port,Data_t data);
void *sendString(void *msg);
void sendDataSTRING(const char* ip,int port);
void run(IrrlichtDevice *device);
void isikEkle();


void sendData( int sockfd, void* data) {
	if ( (write( sockfd, (data), sizeof(Data_t)  ) < 0 ) )
		printf("ERROR writing to socket");
}

void sendData2( int sockfd, void* data) {
	if ( (write( sockfd, (data), (sizeof(char)*BUFSIZ)  ) < 0 ) )
		printf("ERROR writing to socket");
}


class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(SAppContext & context) : Context(context) { 
		for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();

			switch(event.GUIEvent.EventType)
			{

				case EGET_BUTTON_CLICKED:
					switch(id)
					{
						case GUI_ID_QUIT_BUTTON:
							Context.device->closeDevice();
							return true;

						case GUI_ID_GIT:
							{
								gitButtonClicked(1);
								str = L"BILGI EKRANI";
							}
							return true;

						case GUI_ID_BASLANGICAYARLA:
							{
								
								gitButtonClicked(2);
	
								
							}
							return true;


						default:
							return false;
					}
					break;

				default:
					break;
			}
		}

		return false;
	}

	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}

private:
	SAppContext & Context;
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
};


void Simulation::initializeAndRun()
{

	IrrlichtDevice * device = createDevice(video::EDT_OPENGL, core::dimension2d<u32>(640, 500));

	PORT = atoi(argv[2]);
	IP = argv[1];

	if (device == 0)
		 exit(1); // could not create selected driver.

	/* The creation was successful, now we set the event receiver and
		store pointers to the driver and to the gui environment. */

	device->setWindowCaption(L"InSchool Robot - Simulation");
	device->setResizable(true);


	driver = device->getVideoDriver();
	

	/*
	To make the font a little bit nicer, we load an external font
	and set it as the new default font in the skin.
	To keep the standard font for tool tip text, we set it to
	the built-in font.
	*/


	smgr = device->getSceneManager();

	scene::ITriangleSelector* selector = 0 ;

	isikEkle();

	// Creates the mesh
	mesh = device->getSceneManager()->getMesh( "kat.obj" );

	for (unsigned int i=0; i<mesh->getMeshBufferCount(); i++) {
        mesh->getMeshBuffer(i)->setHardwareMappingHint(scene::EHM_STATIC);
    }

	// Create animated scene node.
	bina = smgr->addMeshSceneNode(mesh);

	bina->setPosition(core::vector3df(0, 0, 0));
	bina->setScale(core::vector3df(1,1,1));

	selector = smgr->createOctreeTriangleSelector(bina->getMesh(),bina,1024);
	bina->setTriangleSelector(selector);


	// Creates the mesh
	meshPlayer = device->getSceneManager()->getMesh( "araba.obj" );

	for (unsigned int i=0; i<meshPlayer->getMeshBufferCount(); i++) {
        meshPlayer->getMeshBuffer(i)->setHardwareMappingHint(scene::EHM_STATIC);
    }

	// Create animated scene node.
	player = smgr->addMeshSceneNode(meshPlayer);

	player->setPosition(core::vector3df(200, KAT_Y[1], 1475));
	player->setScale(core::vector3df(3,3,3));
	player->setRotation(core::vector3df(0, -90, 0));


	/*
	To be able to look at and move around in this scene, we create a first
	person shooter style camera and make the mouse cursor invisible.
	*/
	cam = smgr->addCameraSceneNode();
	device->getCursorControl()->setVisible(true);

	camPosition = player->getPosition();
	camPosition.Y += 60;
	camPosition.X -= 150;
	cam->setPosition(camPosition);


	cam->bindTargetAndRotation(true);
	cam->setRotation(core::vector3df(0, -90, 0));
	cam->setTarget(player->getPosition());


	env = device->getGUIEnvironment();

	GUIAdd();
     
	// Store the appropriate data in a context structure.
	SAppContext context;
	context.device = device;
	context.counter = 0;
	MyEventReceiver receiver(context);
	device->setEventReceiver(&receiver);

	pthread_create( &portThread, NULL, dataControlAndProccess, NULL);
	pthread_create( &BPlaniThread, NULL, sendString, NULL);

	str = L"BILGI EKRANI"; 


	run(device);

	device->drop();

}


void run(IrrlichtDevice *device){


	int status = 0;

	while(device->run() && driver){


		driver->beginScene(true, true, video::SColor(255,113,113,113));

        smgr->drawAll();
        env->drawAll();

        driver->endScene();


		playerPosition = player->getPosition();
		camPosition = cam->getPosition();

			//printf("%f %f %f\n",playerPosition.X,playerPosition.Y,playerPosition.Z);

		if(XPos_Positive > 0.05 ){
			if(BPlaniPanik == 0){
				runControl = 1;
				if(status == 0){
					camPosition = player->getPosition();
					camPosition.Y += 60;
					camPosition.X -= 150;
					rotation(robotFront, XPOS, camPosition);
					
					cam->setPosition(camPosition);
					
					cam->setTarget(player->getAbsolutePosition());
					cam->setRotation(core::vector3df(0, 90, 0));
					status = 1;
				}
					
				
				playerPosition.X += HIZ;
				player->setPosition(playerPosition);
				cam->setTarget(player->getAbsolutePosition());
				camPosition.X += HIZ;
				cam->setPosition(camPosition);
					
				
				XPos_Positive-- ;

				if(XPos_Positive < 0.05){
					runControl = 0;
					status = 0 ;
				}
			}
		}

		if(XPos_Negative > 0.05 ){
			if(BPlaniPanik == 0){
				runControl = 1;
				if(status == 0){
					camPosition = player->getPosition();
					camPosition.Y += 60;
					camPosition.X += 150;

					rotation(robotFront, XNEG,camPosition);
					
					cam->setPosition(camPosition);
					
					cam->setTarget(player->getAbsolutePosition());
				    cam->setRotation(core::vector3df(0, -90, 0));
					status = 1;
				}

				driver->beginScene(true, true, video::SColor(255,113,113,133));
				playerPosition.X -= HIZ;
				player->setPosition(playerPosition);
				cam->setTarget(player->getAbsolutePosition());
				camPosition.X -= HIZ;
				cam->setPosition(camPosition);
				
				
				XPos_Negative-- ;

				if(XPos_Negative < 0.05){
					runControl = 0;
					status = 0 ;
				}
			}
		}

		if(ZPos_Positive > 0.05){
			if(BPlaniPanik == 0){
				runControl = 1;
				if(status == 0){
					camPosition = player->getPosition();
					camPosition.Y += 60;
					camPosition.Z -= 150;

					rotation(robotFront, ZPOS, camPosition);
					
					cam->setPosition(camPosition);
					
					cam->setTarget(player->getAbsolutePosition());
					cam->setRotation(core::vector3df(0, 360, 0));
					status = 1;
				}

				driver->beginScene(true, true, video::SColor(255,113,113,133));
				playerPosition.Z += HIZ;
				player->setPosition(playerPosition);
				cam->setTarget(player->getAbsolutePosition());	
				camPosition.Z += HIZ;
				cam->setPosition(camPosition);
				
				ZPos_Positive-- ;

				if(ZPos_Positive < 0.05){
					runControl = 0;
					status = 0 ;
				}
			}
		}

		if(ZPos_Negative > 0.05){
			if(BPlaniPanik == 0){
				runControl = 1;

				if(status == 0){
					camPosition = player->getPosition();
					camPosition.Y += 60;
					camPosition.Z += 150;

					rotation(robotFront, ZNEG, camPosition);
					
					cam->setPosition(camPosition);
					
					cam->setTarget(player->getPosition());
					cam->setRotation(core::vector3df(0, 180, 0));

					status = 1;
				}
					

				driver->beginScene(true, true, video::SColor(255,113,113,133));
				playerPosition.Z -= HIZ;
				player->setPosition(playerPosition);
				cam->setTarget(player->getPosition());
				camPosition.Z -= HIZ;
				cam->setPosition(camPosition);
				
				ZPos_Negative-- ;
				if(ZPos_Negative < 0.05){
					runControl = 0;
					status = 0 ;
				}
			}
		}

		if(BPlaniPanik == 1){
			runControl = 1;
			if(DEGISIM<0){
				if(playerPosition.Y > KAT_Y[HEDEFKAT]){
					playerPosition.Y += DEGISIM/100;
					camPosition.Y += DEGISIM/100;
					player->setPosition(playerPosition);
					cam->setPosition(camPosition);
					cam->setTarget(player->getPosition());
				
				}
				else{
					BPlaniPanik = 0;
					runControl = 0;
					playerPosition.Y = KAT_Y[HEDEFKAT];
					player->setPosition(playerPosition);
					BULUNDUGUM_KAT = HEDEFKAT;
					str = L"DEVAM'A BASIN";
				}
			}

			if(DEGISIM>0){
				if(playerPosition.Y < KAT_Y[HEDEFKAT]){
					playerPosition.Y += DEGISIM/100;
					camPosition.Y += DEGISIM/100;
					player->setPosition(playerPosition);
					cam->setPosition(camPosition);
					cam->setTarget(player->getPosition());
				
				}
				else{
					BPlaniPanik = 0;
					runControl = 0;
					playerPosition.Y = KAT_Y[HEDEFKAT];
					player->setPosition(playerPosition);
					BULUNDUGUM_KAT = HEDEFKAT;
					str = L"DEVAM'A BASIN";
					
				}
			}
			if(HEDEFKAT == 0)
				Current	= BASLANGIC0;
			if(HEDEFKAT == 1)
				Current	= BASLANGIC1;
			if(HEDEFKAT == 2)
				Current	= BASLANGIC2;

		}

		if(playerPosition.Z > 1350){
			str = L"KAT:";
			str += BULUNDUGUM_KAT; 
			str += L" > DOGU KORIDORU";

			char a[2];
			sprintf(a,"%d",BULUNDUGUM_KAT);
			StringDATA[0]= '\0';
			strcat(StringDATA,"KAT:");
			strcat(StringDATA,a);
			strcat(StringDATA," > DOGU KORIDORU");
		}
		if(playerPosition.Z < -860){
			str = L"KAT:";
			str += BULUNDUGUM_KAT; 
			str += L" > BATI KORIDORU"; 

			char a[2];
			sprintf(a,"%d",BULUNDUGUM_KAT);
			StringDATA[0]= '\0';
			strcat(StringDATA,"KAT:");
			strcat(StringDATA,a);
			strcat(StringDATA," > BATI KORIDORU");

		}
		if(playerPosition.Z > -860 && playerPosition.Z < 1350 && playerPosition.X < -480){
			str = L"KAT:";
			str += BULUNDUGUM_KAT; 
			str += L" > KUZEY KORIDORU"; 

			char a[2];
			sprintf(a,"%d",BULUNDUGUM_KAT);
			StringDATA[0]= '\0';
			strcat(StringDATA,"KAT:");
			strcat(StringDATA,a);
			strcat(StringDATA," > KUZEY KORIDORU");
		}
		if(playerPosition.Z > -860 && playerPosition.Z < 1350 && playerPosition.X > 780){
			str = L"KAT:";
			str += BULUNDUGUM_KAT; 
			str += L" > GUNEY KORIDORU"; 

			char a[2];
			sprintf(a,"%d",BULUNDUGUM_KAT);
			StringDATA[0]= '\0';
			strcat(StringDATA,"KAT:");
			strcat(StringDATA,a);
			strcat(StringDATA," > GUNEY KORIDORU");
		}
		
		myTextBox->setText(str.c_str());

	}
}

void gitButtonClicked(int clicked){
	
	int select;
	if(clicked == 1){
		HIZ = 0.82 ; 
		MetreBUYUTME = 96.34;
		select = box2->getSelected();
	}

	if(clicked == 2){
		HIZ = 20 ; 
		MetreBUYUTME = 3.95;
		select = box->getSelected();
	}

	switch(select){
		case 0: Next = BASLANGIC0;
				break;
		case 1: Next = Z23;
				break;
		case 2: Next = Z20;
				break;
		case 3: Next = Z12;
				break;
		case 4: Next = Z10;
				break;
		case 5: Next = Z05;
				break;
		case 6: Next = Z04;
				break;
		case 7: Next = Z02;
				break;
		case 8: Next = ROBOTIK;
				break;
		case 9: Next = Z06;
				break;
		case 10: Next = Z11;
				break;
		case 11: Next = GENEL_LAB;
				break;
		case 12: Next = KANTIN;
				break;
		case 13: Next = BASLANGIC1;
				break;
		case 14: Next = N123;
				break;
		case 15: Next = N121;
				break;
		case 16: Next = N119;
				break;
		case 17: Next = N109;
				break;
		case 18: Next = N104;
				break;
		case 19: Next = N103;
				break;
		case 20: Next = N102;
				break;
		case 21: Next = N101;
				break;
		case 22: Next = N108;
				break;
		case 23: Next = N118;
				break;
		case 24: Next = N120;
				break;
		case 25: Next = N122;
				break;
		case 26: Next = N24;
				break;
		case 27: Next = BASLANGIC2;
				break;
		case 28: Next = N254;
				break;
		case 29: Next = N255;
				break;
		case 30: Next = N248;
				break;
		case 31: Next = N208;
				break;
		case 32: Next = N243;
				break;
		case 33: Next = N244;
				break;
		case 34: Next = N239;
				break;
		case 35: Next = N240;
				break;
		case 36: Next = N235;
				break;
		case 37: Next = N234;
				break;
		case 38: Next = N230;
				break;
		case 39: Next = N226;
				break;
		case 40: Next = N221;
				break;
		case 41: Next = N220;
				break;
		case 42: Next = N216;
				break;
		case 43: Next = N215;
				break;
		case 44: Next = N212;
				break;
		case 45: Next = N207;
				break;
		case 46: Next = N206;
				break;
		case 47: Next = N205;
				break;
		case 48: Next = N204;
				break;
		case 49: Next = N203;
				break;
		case 50: Next = N202;
				break;
		case 51: Next = N201;
				break;
		case 52: Next = N214;
				break;
		case 53: Next = N213;
				break;
		case 54: Next = N219;
				break;
		case 55: Next = N218;
				break;
		case 56: Next = N225;
				break;
		case 57: Next = N229;
				break;
		case 58: Next = N233;
				break;
		case 59: Next = N232;
				break;
		case 60: Next = N237;
				break;
		case 61: Next = N238;
				break;
		case 62: Next = N241;
				break;
		case 63: Next = N242;
				break;
		case 64: Next = N251;
				break;
		case 65: Next = N252;
				break;
		case 66: Next = N253;
				break;
	
	}


	data=calculateRoute(Current,Next); 
	Current=Next;

	if(MetreBUYUTME > 60){
		sendDataT(IP,PORT,data);
	}
}

void rotation(FRONT on, FRONT yeniOn, vector3df hedefPosition){
	
	int rotateNum = 0,rotateState,k,camRotate=0;
	switch(on) {
		case XPOS:
			if(yeniOn == XNEG){
				rotateNum = 180;
				rotateState = 0;
				camRotate = 1;
			}	
			else if(yeniOn == ZPOS){
				rotateNum = 90;
				rotateState = 1;
				camRotate = 2;
			}	
			else if(yeniOn == ZNEG){
				rotateNum = 90;
				rotateState = 0;
				camRotate = 1;
			
			}	
			break;
		case XNEG:
			if(yeniOn == XPOS){
				rotateNum = 180;
				rotateState = 0;
				camRotate = 3;
			
			}	
			else if(yeniOn == ZPOS){
				rotateNum = 90;
				rotateState = 0;
				camRotate = 3;
			
			}	
			else if(yeniOn == ZNEG){
				rotateNum = 90;
				rotateState = 1;
				camRotate = 4;
			
			}	
			break;
		case ZPOS:
			if(yeniOn == XPOS){
				rotateNum = 90;
				rotateState = 0;
				camRotate = 4;
			}	
			else if(yeniOn == XNEG){
				rotateNum = 90;
				rotateState = 1;
				camRotate = 1;
			}	
			else if(yeniOn == ZNEG){
				rotateNum = 180;
				rotateState = 0;
				camRotate = 4;
			}	
			break;
		case ZNEG:
			if(yeniOn == XPOS){
				rotateNum = 90;
				rotateState = 1;
				camRotate = 3;
			}	
			else if(yeniOn == XNEG){
				rotateNum = 90;
				rotateState = 0;
				camRotate = 2;
			}	
			else if(yeniOn == ZPOS){
				rotateNum = 180;
				rotateState = 0;
				camRotate = 2;
			}	
			break;
		default:
			exit(0);
	}


	if(rotateNum!=0 && MetreBUYUTME > 60)
		sleep(2);

				
	cameraRotation = cam->getPosition();
	playerRotation = player->getRotation();
	double zArtir;
	double xArtir;
	if(rotateNum != 0){
		zArtir = (hedefPosition.Z - cameraRotation.Z) / rotateNum;
		xArtir = (hedefPosition.X - cameraRotation.X) / rotateNum;
	}
	if(zArtir<0)
		zArtir *= -1;
	if(xArtir<0)
		xArtir *= -1;
	for(int j = playerRotation.Y ; j<playerRotation.Y+rotateNum ; j++){
		
	 	driver->beginScene(true, true, video::SColor(150,50,50,50));

        smgr->drawAll();
        env->drawAll();

        driver->endScene();
		if(rotateState == 1){
			player->setRotation(core::vector3df(0, -j, 0));
			if(MetreBUYUTME < 60){
				player->setRotation(core::vector3df(0, -playerRotation.Y+rotateNum, 0));
				break ;
			}
					
			//cam->setRotation(player->getRotation());

		}
		else{
			player->setRotation(core::vector3df(0, j, 0));
			if(MetreBUYUTME < 60){
				player->setRotation(core::vector3df(0, playerRotation.Y+rotateNum, 0));
				break ;
			}
					
			
			//cam->setRotation(-player->getRotation());
		}

		if(camRotate == 1){
			cameraRotation.Z+= zArtir;
			cameraRotation.X+= xArtir;
		}
		if(camRotate == 2){
			cameraRotation.Z-= zArtir;
			cameraRotation.X+= xArtir;
		}	
		if(camRotate == 3){
			cameraRotation.X-= xArtir;
			cameraRotation.Z-= zArtir;
		}
		if(camRotate == 4){
			cameraRotation.X-= xArtir;
			cameraRotation.Z+= zArtir;
		}
		cam->setPosition(cameraRotation);
		cam->setTarget(player->getPosition());
		
	}

	robotFront = yeniOn;
}

void *dataControlAndProccess(void* msg){

	int i;

	Direction ROTATE;
	int rotateControl;

	while(1){
		if(data.size > 0){
			for(i=0 ; i<data.size ; i++){
				printf("%d - %f \n",data.route[i].way,data.route[i].step);

				if(data.route[i].step > 85 && data.route[i].step < 100){
					ROTATE = data.route[i].way;
					rotateControl = 1;
					
				}

				else if(data.route[i].step > 175){
					ROTATE = BACK;
					rotateControl = 1;
					
				}
				if(rotateControl == 1){	
					data.route[i].way = ROTATE;
					data.route[i].step = 0.1;
					rotateControl = 0;
				}


				switch(data.route[i].way) {
					case FORWARD:
						if(robotFront == XPOS)
							XPos_Positive = (data.route[i].step* MetreBUYUTME);
						else if(robotFront == XNEG)
							XPos_Negative = (data.route[i].step* MetreBUYUTME);
						else if(robotFront == ZPOS)
							ZPos_Positive = (data.route[i].step* MetreBUYUTME);
						else if(robotFront == ZNEG)
							ZPos_Negative = (data.route[i].step* MetreBUYUTME);
						break;
					case BACK:
						if(robotFront == XPOS)
							XPos_Negative = (data.route[i].step* MetreBUYUTME);
						else if(robotFront == XNEG)
							XPos_Positive = (data.route[i].step* MetreBUYUTME);
						else if(robotFront == ZPOS)
							ZPos_Negative = (data.route[i].step* MetreBUYUTME);
						else if(robotFront == ZNEG)
							ZPos_Positive = (data.route[i].step* MetreBUYUTME);
						break;
					case LEFT:
						if(robotFront == XPOS)
							ZPos_Positive = (data.route[i].step* MetreBUYUTME);
						else if(robotFront == XNEG)
							ZPos_Negative = (data.route[i].step* MetreBUYUTME);
						else if(robotFront == ZPOS)
							XPos_Negative = (data.route[i].step* MetreBUYUTME);
						else if(robotFront == ZNEG)
							XPos_Positive = (data.route[i].step* MetreBUYUTME);
						break;
					case RIGHT:
						if(robotFront == XPOS)
							ZPos_Negative = (data.route[i].step* MetreBUYUTME);
						else if(robotFront == XNEG)
							ZPos_Positive = (data.route[i].step* MetreBUYUTME);
						else if(robotFront == ZPOS)
							XPos_Positive = (data.route[i].step* MetreBUYUTME);
						else if(robotFront == ZNEG)
							XPos_Negative = (data.route[i].step* MetreBUYUTME);
						break;

					case KATDEGISTIR:
						BPlaniPanik = 1;
						HEDEFKAT = data.route[i].step; 
						DEGISIM = KAT_Y[HEDEFKAT] - KAT_Y[BULUNDUGUM_KAT];

						break;
					default:
						exit(0);
						
				}
				runControl = 1;
				while(runControl == 1);

			}
			data.size = 0;
		}
	}

}


void sendDataT(char* ip,int port,Data_t data){
	int sock;
    struct sockaddr_in server;
     
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons( port );
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
    	printf("Robota veri yollanmadi!! \n");
    }
    else{
    	sendData( sock, &data );
    }


     
    close(sock);
}

void *sendString(void *msg){
	while(1){
		sendDataSTRING("127.0.0.1",7578);
		sleep(3);
	}
	

};


void sendDataSTRING(const char* ip,int port){
	int sock;
    struct sockaddr_in server;
     
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons( port );
 
    //Connect to remote server
    while (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0);
     
    puts("Connected\n");
     
    sendData2( sock, &StringDATA );
	StringDATA[0] = '\0';
   
     
    close(sock);
}

void GUIAdd(){
	IGUISkin* skin = env->getSkin();
    IGUIFont* font = env->getFont("fontcourier.bmp");
    if (font)
        skin->setFont(font);

    skin->setFont(env->getBuiltInFont(), EGDF_TOOLTIP);

	box = env->addComboBox(core::rect<s32>(10,400,130,420), 0, GUI_ID_COMBO1);
	box->addItem(L"BASLANGIC0");
    box->addItem(L"Z23");
    box->addItem(L"Z20"); 
    box->addItem(L"Z12");
    box->addItem(L"Z10");
    box->addItem(L"Z05");
    box->addItem(L"Z04");
    box->addItem(L"Z02");
    box->addItem(L"ROBOTIK");
    box->addItem(L"Z06");
    box->addItem(L"Z11");
    box->addItem(L"GENEL_LAB");
    box->addItem(L"KANTIN");

    box->addItem(L"BASLANGIC1");
    box->addItem(L"N123");
    box->addItem(L"N121"); 
    box->addItem(L"N119");
    box->addItem(L"N109");
    box->addItem(L"N104");
    box->addItem(L"N103");
    box->addItem(L"N102");
    box->addItem(L"N101");
    box->addItem(L"N108");
    box->addItem(L"N118");
    box->addItem(L"N120");
    box->addItem(L"N122");
    box->addItem(L"N24");

    box->addItem(L"BASLANGIC2");
    box->addItem(L"N254"); 
    box->addItem(L"N255");
    box->addItem(L"N248");
    box->addItem(L"N208");
    box->addItem(L"N243");
    box->addItem(L"N244");
    box->addItem(L"N239");
    box->addItem(L"N240");
    box->addItem(L"N235");
    box->addItem(L"N234");
    box->addItem(L"N230");
    box->addItem(L"N226");
    box->addItem(L"N221");
    box->addItem(L"N220"); 
    box->addItem(L"N216");
    box->addItem(L"N215");
    box->addItem(L"N212");
    box->addItem(L"N207");
    box->addItem(L"N206");
    box->addItem(L"N205");
    box->addItem(L"N204");
    box->addItem(L"N203");
    box->addItem(L"N202");
    box->addItem(L"N201");
    box->addItem(L"N214");
    box->addItem(L"N213"); 
    box->addItem(L"N219");
    box->addItem(L"N218");
    box->addItem(L"N225");
    box->addItem(L"N229");
    box->addItem(L"N233");
    box->addItem(L"N232");
    box->addItem(L"N237");
    box->addItem(L"N238");
    box->addItem(L"N241");
    box->addItem(L"N242");
    box->addItem(L"N251");
    box->addItem(L"N252");
    box->addItem(L"N253"); 
 

    env->addButton(rect<s32>(140,400,290,420), 0, GUI_ID_BASLANGICAYARLA,
			L"BASLANGIC AYARLA");

    box2 = env->addComboBox(core::rect<s32>(10,430,130,450), 0, GUI_ID_COMBO2);
    box2->addItem(L"BASLANGIC0");
    box2->addItem(L"Z23");
    box2->addItem(L"Z20"); 
    box2->addItem(L"Z12");
    box2->addItem(L"Z10");
    box2->addItem(L"Z05");
    box2->addItem(L"Z04");
    box2->addItem(L"Z02");
    box2->addItem(L"ROBOTIK");
    box2->addItem(L"Z06");
    box2->addItem(L"Z11");
    box2->addItem(L"GENEL_LAB");
    box2->addItem(L"KANTIN");

    box2->addItem(L"BASLANGIC1");
    box2->addItem(L"N123");
    box2->addItem(L"N121"); 
    box2->addItem(L"N119");
    box2->addItem(L"N109");
    box2->addItem(L"N104");
    box2->addItem(L"N103");
    box2->addItem(L"N102");
    box2->addItem(L"N101");
    box2->addItem(L"N108");
    box2->addItem(L"N118");
    box2->addItem(L"N120");
    box2->addItem(L"N122");
    box2->addItem(L"N24");

    box2->addItem(L"BASLANGIC2");
    box2->addItem(L"N254"); 
    box2->addItem(L"N255");
    box2->addItem(L"N248");
    box2->addItem(L"N208");
    box2->addItem(L"N243");
    box2->addItem(L"N244");
    box2->addItem(L"N239");
    box2->addItem(L"N240");
    box2->addItem(L"N235");
    box2->addItem(L"N234");
    box2->addItem(L"N230");
    box2->addItem(L"N226");
    box2->addItem(L"N221");
    box2->addItem(L"N220"); 
    box2->addItem(L"N216");
    box2->addItem(L"N215");
    box2->addItem(L"N212");
    box2->addItem(L"N207");
    box2->addItem(L"N206");
    box2->addItem(L"N205");
    box2->addItem(L"N204");
    box2->addItem(L"N203");
    box2->addItem(L"N202");
    box2->addItem(L"N201");
    box2->addItem(L"N214");
    box2->addItem(L"N213"); 
    box2->addItem(L"N219");
    box2->addItem(L"N218");
    box2->addItem(L"N225");
    box2->addItem(L"N229");
    box2->addItem(L"N233");
    box2->addItem(L"N232");
    box2->addItem(L"N237");
    box2->addItem(L"N238");
    box2->addItem(L"N241");
    box2->addItem(L"N242");
    box2->addItem(L"N251");
    box2->addItem(L"N252");
    box2->addItem(L"N253"); 


	env->addButton(core::rect<s32>(140,430,290,450), 0, GUI_ID_GIT, L"GIT");

    env->addButton(core::rect<s32>(300,430,420,450), 0, GUI_ID_QUIT_BUTTON, L"CIKIS");

    env->addButton(core::rect<s32>(10,460,130,480), 0, GUI_ID_GIT, L"DEVAM");

    myTextBox = env->addStaticText(L"", rect<s32>(300,400,500,420), true);
}


void isikEkle(){
	int isikParlaklik = 400;
	int xPos= 680,
		xNeg= -450,
		zPos= 1500,
		zNeg= -900;

	smgr->addLightSceneNode(0, core::vector3df(xPos,400,zPos),
		video::SColorf(1.0f,1.0f,1.0f),isikParlaklik);
	smgr->addLightSceneNode(0, core::vector3df(xNeg,400,zPos),
		video::SColorf(1.0f,1.0f,1.0f),isikParlaklik);
	smgr->addLightSceneNode(0, core::vector3df(xPos,400,zNeg),
		video::SColorf(1.0f,1.0f,1.0f),isikParlaklik);
	smgr->addLightSceneNode(0, core::vector3df(xNeg,400,zNeg),
		video::SColorf(1.0f,1.0f,1.0f),isikParlaklik);

	smgr->addLightSceneNode(0, core::vector3df(xPos,800,zPos),
		video::SColorf(1.0f,1.0f,1.0f),isikParlaklik);
	smgr->addLightSceneNode(0, core::vector3df(xNeg,800,zPos),
		video::SColorf(1.0f,1.0f,1.0f),isikParlaklik);
	smgr->addLightSceneNode(0, core::vector3df(xPos,800,zNeg),
		video::SColorf(1.0f,1.0f,1.0f),isikParlaklik);
	smgr->addLightSceneNode(0, core::vector3df(xNeg,800,zNeg),
		video::SColorf(1.0f,1.0f,1.0f),isikParlaklik);

	smgr->addLightSceneNode(0, core::vector3df(xPos,1400,zPos),
		video::SColorf(1.0f,1.0f,1.0f),isikParlaklik);
	smgr->addLightSceneNode(0, core::vector3df(xNeg,1400,zPos),
		video::SColorf(1.0f,1.0f,1.0f),isikParlaklik);
	smgr->addLightSceneNode(0, core::vector3df(xPos,1400,zNeg),
		video::SColorf(1.0f,1.0f,1.0f),isikParlaklik);
	smgr->addLightSceneNode(0, core::vector3df(xNeg,1400,zNeg),
		video::SColorf(1.0f,1.0f,1.0f),isikParlaklik);
}
