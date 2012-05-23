#include <OpenHome/OhNetTypes.h>
#include <OpenHome/Net/Core/OhNet.h>
#include <OpenHome/Private/Ascii.h>
#include <OpenHome/Private/Maths.h>
#include <OpenHome/Net/Private/Stack.h>
#include <OpenHome/Private/Thread.h>
#include <OpenHome/Private/OptionParser.h>
#include <OpenHome/Private/Debug.h>
#include <OpenHome/Private/TestFramework.h>

#include <vector>
#include <stdio.h>

#include "ReceiverManager1.h"


#ifdef _WIN32
#define CDECL __cdecl
#else
#define CDECL 
#endif


namespace OpenHome {
namespace Av {

	class ReceiverManager1Logger : IReceiverManager1Handler
	{
	public:
		ReceiverManager1Logger();
		virtual void ReceiverAdded(ReceiverManager1Receiver& aReceiver);
		virtual void ReceiverChanged(ReceiverManager1Receiver& aReceiver);
		virtual void ReceiverRemoved(ReceiverManager1Receiver& aReceiver);
		virtual void ReceiverVolumeControlChanged(ReceiverManager1Receiver& aReceiver);
		virtual void ReceiverVolumeChanged(ReceiverManager1Receiver& aReceiver);
		virtual void ReceiverMuteChanged(ReceiverManager1Receiver& aReceiver);
		virtual void ReceiverVolumeLimitChanged(ReceiverManager1Receiver& aReceiver);
		~ReceiverManager1Logger();
	private:
		ReceiverManager1* iReceiverManager;
	};

} // namespace Av
} // namespace OpenHome

using namespace OpenHome;
using namespace OpenHome::Net;
using namespace OpenHome::TestFramework;
using namespace OpenHome::Av;

ReceiverManager1Logger::ReceiverManager1Logger()
{
	iReceiverManager = new ReceiverManager1(*this);
}

ReceiverManager1Logger::~ReceiverManager1Logger()
{
	delete (iReceiverManager);
}

void ReceiverManager1Logger::ReceiverAdded(ReceiverManager1Receiver& aReceiver)
{
    Print("Added   ");
    Print(aReceiver.Room());
    Print("(");
    Print(aReceiver.Group());
    Print(")");
	if (aReceiver.Selected()) {
	    Print(" - Selected");
	}
    Print("\n");
}

void ReceiverManager1Logger::ReceiverChanged(ReceiverManager1Receiver& aReceiver)
{
    Print("Changed   ");
    Print(aReceiver.Room());
    Print("(");
    Print(aReceiver.Group());
    Print(")");
	if (aReceiver.Selected()) {
	    Print(" - Selected");
	}
    Print("\n");
}

void ReceiverManager1Logger::ReceiverRemoved(ReceiverManager1Receiver& aReceiver)
{
    Print("Removed ");
    Print(aReceiver.Room());
    Print("(");
    Print(aReceiver.Group());
    Print(")\n");
}

void ReceiverManager1Logger::ReceiverVolumeControlChanged(ReceiverManager1Receiver& aReceiver)
{
	Print("Vol Control Changed ");
	Print(aReceiver.Room());
	Print("(");
	Print(aReceiver.Group());
	Print("): ");
    aReceiver.HasVolumeControl() ? printf("Yes\n") : printf("No\n");
	if(aReceiver.HasVolumeControl())
	{
		Print("Vol      ");
		Bws<Ascii::kMaxUintStringBytes> bufferVol;
		Ascii::AppendDec(bufferVol, aReceiver.Volume());
		Print(bufferVol);
		Print("\n");
		Print("Mute      ");
		Bws<Ascii::kMaxUintStringBytes> bufferMute;
		Ascii::AppendDec(bufferMute, aReceiver.Mute());
		Print(bufferMute);
		Print("\n");
		Print("Vol Limit      ");
		Bws<Ascii::kMaxUintStringBytes> bufferVolLim;
		Ascii::AppendDec(bufferVolLim, aReceiver.VolumeLimit());
		Print(bufferVolLim);
		Print("\n");
	}
}

void ReceiverManager1Logger::ReceiverVolumeChanged(ReceiverManager1Receiver& aReceiver)
{
	Print("Vol Changed ");
	Print(aReceiver.Room());
	Print("(");
	Print(aReceiver.Group());
	Print("): ");
	Bws<Ascii::kMaxUintStringBytes> buffer;
	Ascii::AppendDec(buffer, aReceiver.Volume());
    Print(buffer);
    Print("\n");
}

void ReceiverManager1Logger::ReceiverMuteChanged(ReceiverManager1Receiver& aReceiver)
{
	Print("Mute Changed ");
	Print(aReceiver.Room());
	Print("(");
	Print(aReceiver.Group());
	Print("): ");
	Bws<Ascii::kMaxUintStringBytes> buffer;
	Ascii::AppendDec(buffer, aReceiver.Mute());
    Print(buffer);
    Print("\n");
}

void ReceiverManager1Logger::ReceiverVolumeLimitChanged(ReceiverManager1Receiver& aReceiver)
{
	Print("Vol Limit Changed ");
	Print(aReceiver.Room());
	Print("(");
	Print(aReceiver.Group());
	Print("): ");
	Bws<Ascii::kMaxUintStringBytes> buffer;
	Ascii::AppendDec(buffer, aReceiver.VolumeLimit());
    Print(buffer);
    Print("\n");
}

int CDECL main(int aArgc, char* aArgv[])
{
	InitialisationParams* initParams = InitialisationParams::Create();

    OptionParser parser;
    
    OptionUint optionDuration("-d", "--duration", 30, "Number of seconds to run the test");

    parser.AddOption(&optionDuration);
    
    if (!parser.Parse(aArgc, aArgv)) {
        return (1);
    }

    UpnpLibrary::Initialise(initParams);
    std::vector<NetworkAdapter*>* subnetList = UpnpLibrary::CreateSubnetList();
    TIpAddress subnet = (*subnetList)[0]->Subnet();
    UpnpLibrary::DestroySubnetList(subnetList);
    UpnpLibrary::StartCp(subnet);

    // Debug::SetLevel(Debug::kTopology);

	ReceiverManager1Logger* logger = new ReceiverManager1Logger();
	
    Blocker* blocker = new Blocker;
    blocker->Wait(optionDuration.Value());
    delete blocker;
	
	delete (logger);

	Print("Closing ... ");
	UpnpLibrary::Close();
	Print("closed\n");
}
