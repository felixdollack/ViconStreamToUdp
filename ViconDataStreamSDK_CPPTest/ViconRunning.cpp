// ViconDataStreamSDK_CPPTest.cpp : Defines the entry point for the console application.
//
//#using <system.dll>
#include "stdafx.h"
#include "Client.h"

#include <stdio.h>
#include <string.h>
#include <process.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime>
#include <algorithm>

#ifdef WIN32
  #include <conio.h>   // For _kbhit()
  #include <cstdio>   // For getchar()
  #include <windows.h> // For Sleep()
#endif // WIN32

#include <time.h>

using namespace ViconDataStreamSDK::CPP;
using namespace System;
using namespace System::Threading;

//#include <winsock2.h>
//#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#define output_stream if(!LogFile.empty()) ; else std::cout 

namespace
{
  std::string Adapt( const bool i_Value )
  {
    return i_Value ? "True" : "False";
  }

  std::string Adapt( const Direction::Enum i_Direction )
  {
    switch( i_Direction )
    {
      case Direction::Forward:
        return "Forward";
      case Direction::Backward:
        return "Backward";
      case Direction::Left:
        return "Left";
      case Direction::Right:
        return "Right";
      case Direction::Up:
        return "Up";
      case Direction::Down:
        return "Down";
      default:
        return "Unknown";
    }
  }

  std::string Adapt( const DeviceType::Enum i_DeviceType )
  {
    switch( i_DeviceType )
    {
      case DeviceType::ForcePlate:
        return "ForcePlate";
      case DeviceType::Unknown:
      default:
        return "Unknown";
    }
  }

  std::string Adapt( const Unit::Enum i_Unit )
  {
    switch( i_Unit )
    {
      case Unit::Meter:
        return "Meter";
      case Unit::Volt:
        return "Volt";
      case Unit::NewtonMeter:
        return "NewtonMeter";
      case Unit::Newton:
        return "Newton";
      case Unit::Kilogram:
        return "Kilogram";
      case Unit::Second:
        return "Second";
      case Unit::Ampere:
        return "Ampere";
      case Unit::Kelvin:
        return "Kelvin";
      case Unit::Mole:
        return "Mole";
      case Unit::Candela:
        return "Candela";
      case Unit::Radian:
        return "Radian";
      case Unit::Steradian:
        return "Steradian";
      case Unit::MeterSquared:
        return "MeterSquared";
      case Unit::MeterCubed:
        return "MeterCubed";
      case Unit::MeterPerSecond:
        return "MeterPerSecond";
      case Unit::MeterPerSecondSquared:
        return "MeterPerSecondSquared";
      case Unit::RadianPerSecond:
        return "RadianPerSecond";
      case Unit::RadianPerSecondSquared:
        return "RadianPerSecondSquared";
      case Unit::Hertz:
        return "Hertz";
      case Unit::Joule:
        return "Joule";
      case Unit::Watt:
        return "Watt";
      case Unit::Pascal:
        return "Pascal";
      case Unit::Lumen:
        return "Lumen";
      case Unit::Lux:
        return "Lux";
      case Unit::Coulomb:
        return "Coulomb";
      case Unit::Ohm:
        return "Ohm";
      case Unit::Farad:
        return "Farad";
      case Unit::Weber:
        return "Weber";
      case Unit::Tesla:
        return "Tesla";
      case Unit::Henry:
        return "Henry";
      case Unit::Siemens:
        return "Siemens";
      case Unit::Becquerel:
        return "Becquerel";
      case Unit::Gray:
        return "Gray";
      case Unit::Sievert:
        return "Sievert";
      case Unit::Katal:
        return "Katal";

      case Unit::Unknown:
      default:
        return "Unknown";
    }
  }
#ifdef WIN32
  bool Hit()
  {
    bool hit = false;
    while( _kbhit() )
    {
      getchar();
      hit = true;
    }
    return hit;
  }
#endif
}

static unsigned int currentFrame = 0;
static unsigned int frameEvent = 0;
static clock_t pret = clock();
  // Make a new client
  Client MyClient;

//#define TARGETIP "10.0.0.2"
#define TARGETIP "192.168.12.14"
#define TARGETPORT 4444
#define PI 3.14159265359

WSAData wsaData;
WSAData wsaData2;

SOCKET sock;
struct sockaddr_in addr;
int sock_counter=0;

int send_HALsocket(float x, float y, float z, float angle) {
	char abuf[64];
	_snprintf(abuf, sizeof(abuf), "/%lf/%lf/%lf/%lf/\n", ((double)x), ((double)y), ((double)z), ((double)angle));

	sendto(sock, abuf, strlen(abuf), 0, (struct sockaddr *)&addr, sizeof(addr));
	return 0;
}

int close_HALsocket( void ){
   closesocket(sock);

   WSACleanup();
   return 0;
}

int init_HALsocket(void) {
	WSAStartup(MAKEWORD(2, 0), &wsaData);

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(TARGETPORT);
	addr.sin_addr.S_un.S_addr = inet_addr(TARGETIP);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{

	
  std::string HostName = "localhost:801";
	//std::string HostName = "192.168.11.50:801";

  if( argc > 1 )
  {
    HostName = argv[1];
  }
  // log contains:
  // version number
  // log of framerate over time
  // --multicast
  // kill off internal app
  std::string LogFile = "";
  std::string MulticastAddress = "244.0.0.0:44801";
  bool ConnectToMultiCast = false;
  bool EnableMultiCast = false;
  /*for(int a=2; a < argc; ++a)
  {
    std::string arg = argv[a];
    if(arg == "--help")
    {
      std::cout << argv[0] << " <HostName>: allowed options include:\n  --log_file <LogFile> --enable_multicast <MulticastAddress:Port> --connect_to_multicast <MulticastAddress:Port> --help" << std::endl;
      return 0;
    }
    else if (arg=="--log_file")
    {
      if(a < argc)
      {
        LogFile = argv[a+1];
        std::cout << "Using log file <"<< LogFile << "> ..." << std::endl;
        ++a;
      }
    }
    else if (arg=="--enable_multicast")
    {
      EnableMultiCast = true;
      if(a < argc)
      {
        MulticastAddress = argv[a+1];
        std::cout << "Enabling multicast address <"<< MulticastAddress << "> ..." << std::endl;
        ++a;
      }
    }
    else if (arg=="--connect_to_multicast")
    {
      ConnectToMultiCast = true;
      if(a < argc)
      {
        MulticastAddress = argv[a+1];
        std::cout << "connecting to multicast address <"<< MulticastAddress << "> ..." << std::endl;
        ++a;
      }
    }
    else
    {
      std::cout << "Failed to understand argument <" << argv[a] << ">...exiting" << std::endl;
      return 1;
    }
  }*/

  std::ofstream ofs;
  if(!LogFile.empty())
  {
    ofs.open(LogFile.c_str());
    if(!ofs.is_open())
    {
      std::cout << "Could not open log file <" << LogFile << ">...exiting" << std::endl;
      return 1;
    }
  }

  for(int i=0; i != 3; ++i) // repeat to check disconnecting doesn't wreck next connect
  {
    // Connect to a server
    std::cout << "Connecting to " << HostName << " ..." << std::flush;
    while( !MyClient.IsConnected().Connected )
    {
      // Direct connection

      bool ok = false;
      if(ConnectToMultiCast)
      {
        // Multicast connection
        ok = ( MyClient.ConnectToMulticast( HostName, MulticastAddress ).Result == Result::Success );

      }
      else
      {
        ok =( MyClient.Connect( HostName ).Result == Result::Success );
      }
      if(!ok)
      {
        std::cout << "Warning - connect failed..." << std::endl;
      }


      std::cout << ".";
  #ifdef WIN32
      Sleep( 200 );
  #else
      sleep(1);
  #endif
    }
    std::cout << std::endl;

    // Enable some different data types
    MyClient.EnableSegmentData();
    MyClient.EnableMarkerData();

    std::cout << "Segment Data Enabled: "          << Adapt( MyClient.IsSegmentDataEnabled().Enabled )         << std::endl;
    std::cout << "Marker Data Enabled: "           << Adapt( MyClient.IsMarkerDataEnabled().Enabled )          << std::endl;
    std::cout << "Unlabeled Marker Data Enabled: " << Adapt( MyClient.IsUnlabeledMarkerDataEnabled().Enabled ) << std::endl;
    std::cout << "Device Data Enabled: "           << Adapt( MyClient.IsDeviceDataEnabled().Enabled )          << std::endl;


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////create save files
	
	//static FILE *Mhandle=NULL;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Set the streaming mode
    MyClient.SetStreamMode( ViconDataStreamSDK::CPP::StreamMode::ClientPull );

    // Set the global up axis
    MyClient.SetAxisMapping( Direction::Forward, 
                             Direction::Left, 
                             Direction::Up ); // Z-up
    // MyClient.SetGlobalUpAxis( Direction::Forward, 
    //                           Direction::Up, 
    //                           Direction::Right ); // Y-up

    Output_GetAxisMapping _Output_GetAxisMapping = MyClient.GetAxisMapping();
    std::cout << "Axis Mapping: X-" << Adapt( _Output_GetAxisMapping.XAxis ) 
                           << " Y-" << Adapt( _Output_GetAxisMapping.YAxis ) 
                           << " Z-" << Adapt( _Output_GetAxisMapping.ZAxis ) << std::endl;

    // Discover the version number
    Output_GetVersion _Output_GetVersion = MyClient.GetVersion();
    std::cout << "Version: " << _Output_GetVersion.Major << "." 
                             << _Output_GetVersion.Minor << "." 
                             << _Output_GetVersion.Point << std::endl;

    if( EnableMultiCast )
    {
      assert( MyClient.IsConnected().Connected );
      MyClient.StartTransmittingMulticast( HostName, MulticastAddress );
    }

    size_t FrameRateWindow = 1000; // frames
    size_t Counter = 0;
    clock_t LastTime = clock();
    // Loop until a key is pressed

	while( MyClient.GetFrame().Result != Result::Success );
	Sleep(10);
	while( MyClient.GetFrame().Result != Result::Success );
	Sleep(10);
	while( MyClient.GetFrame().Result != Result::Success );
	init_HALsocket();

  #ifdef WIN32
    while( !Hit() )
  #else
    while( true)
  #endif
    {
      // Get a frame
      /*while( MyClient.GetFrame().Result != Result::Success )
      {
		frameEvent = 1;
      }*/
	  if (MyClient.GetFrame().Result == Result::Success) {

      // Get the frame number
      Output_GetFrameNumber _Output_GetFrameNumber = MyClient.GetFrameNumber();
	  currentFrame = _Output_GetFrameNumber.FrameNumber;

	  Output_GetSegmentGlobalTranslation translation = MyClient.GetSegmentGlobalTranslation("Felix", "Head");
	  Output_GetSegmentGlobalRotationEulerXYZ angles = MyClient.GetSegmentGlobalRotationEulerXYZ("Felix", "Head");
	  
	  /*std::cout  << "Frame# " << _Output_GetFrameNumber.FrameNumber;
	  if (translation.Result == Result::Success) {
		std::cout  << " (x/y/z/theta) " << translation.Translation[0] << "/" << translation.Translation[1] << "/" << translation.Translation[2];
	  }
	  if (angles.Result == Result::Success) {
		std::cout  << " " << angles.Rotation[1];///PI*180;
	  }*/
	  send_HALsocket(translation.Translation[0], translation.Translation[1], translation.Translation[2], angles.Rotation[1]);
	  
	/*if (Mhandle == NULL)
    {
		std::time_t result = std::time(NULL);
		const char * mystr=0;
		std::string timestr;
		timestr = std::string("MARKER_DATA_") + std::asctime(std::localtime(&result)) + std::string(".txt");
		timestr.erase(std::remove(timestr.begin(), timestr.end(), '\n'));
		timestr.erase(std::remove(timestr.begin(), timestr.end(), ':'));
		timestr.erase(timestr.end()-1);
		mystr = timestr.data();

		Mhandle = fopen(mystr, "w+");
		fprintf(Mhandle, "Frame");

		for (unsigned int UnlabeledMarkerIndex = 0 ; UnlabeledMarkerIndex < 4 ; ++UnlabeledMarkerIndex)
		{
			fprintf(Mhandle,",M%uX,M%uY,M%uZ",UnlabeledMarkerIndex+1,UnlabeledMarkerIndex+1,UnlabeledMarkerIndex+1);
		}
		fprintf(Mhandle,"\n");
    }

	  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  Marker
	fprintf(Mhandle,"%u",_Output_GetFrameNumber.FrameNumber);

      for( unsigned int UnlabeledMarkerIndex = 0 ; UnlabeledMarkerIndex < 4 ; ++UnlabeledMarkerIndex )
      { 
        // Get the global marker translation
        Output_GetMarkerGlobalTranslation translation = MyClient.GetMarkerGlobalTranslation("Felix", "Head" + std::to_string(UnlabeledMarkerIndex+1));

		// save marker data to the file 
		fprintf(Mhandle,",%.2f,%.2f,%.2f",translation.Translation[ 0 ],translation.Translation[1],translation.Translation[2]);
				
      }
	  fprintf(Mhandle,"\n");*/

	  //std::cout << std::endl;
	  }
    }

    if( EnableMultiCast )
    {
      MyClient.StopTransmittingMulticast();
    }
    MyClient.DisableSegmentData();
    MyClient.DisableMarkerData();
    MyClient.DisableUnlabeledMarkerData();
    MyClient.DisableDeviceData();

    // Disconnect and dispose
    int t = clock();
    std::cout << " Disconnecting..." << std::endl;
    MyClient.Disconnect();
    int dt = clock() - t;
    double secs = (double) (dt)/(double)CLOCKS_PER_SEC;
    std::cout << " Disconnect time = " << secs << " secs" << std::endl;

  }
	close_HALsocket();
	return 0;
}