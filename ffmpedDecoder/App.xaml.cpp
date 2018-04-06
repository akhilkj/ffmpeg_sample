//
// App.xaml.cpp
// Implementation of the App class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace ffmpedDecoder;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();
    Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e)
{
    auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);

    // Do not repeat app initialization when the Window already has content,
    // just ensure that the window is active
    if (rootFrame == nullptr)
    {
        // Create a Frame to act as the navigation context and associate it with
        // a SuspensionManager key
        rootFrame = ref new Frame();

        rootFrame->NavigationFailed += ref new Windows::UI::Xaml::Navigation::NavigationFailedEventHandler(this, &App::OnNavigationFailed);

        if (e->PreviousExecutionState == ApplicationExecutionState::Terminated)
        {
            // TODO: Restore the saved session state only when appropriate, scheduling the
            // final launch steps after the restore is complete

        }

        if (e->PrelaunchActivated == false)
        {
            if (rootFrame->Content == nullptr)
            {
                // When the navigation stack isn't restored navigate to the first page,
                // configuring the new page by passing required information as a navigation
                // parameter
                rootFrame->Navigate(TypeName(MainPage::typeid), e->Arguments);
            }
            // Place the frame in the current Window
            Window::Current->Content = rootFrame;
            // Ensure the current window is active
            Window::Current->Activate();
        }
    }
    else
    {
        if (e->PrelaunchActivated == false)
        {
            if (rootFrame->Content == nullptr)
            {
                // When the navigation stack isn't restored navigate to the first page,
                // configuring the new page by passing required information as a navigation
                // parameter
                rootFrame->Navigate(TypeName(MainPage::typeid), e->Arguments);
            }
            // Ensure the current window is active
            Window::Current->Activate();
        }
    }
}

/// <summary>
/// Invoked when application execution is being suspended.  Application state is saved
/// without knowing whether the application will be terminated or resumed with the contents
/// of memory still intact.
/// </summary>
/// <param name="sender">The source of the suspend request.</param>
/// <param name="e">Details about the suspend request.</param>
void App::OnSuspending(Object^ sender, SuspendingEventArgs^ e)
{
    (void) sender;  // Unused parameter
    (void) e;   // Unused parameter

    //TODO: Save application state and stop any background activity
}

/// <summary>
/// Invoked when Navigation to a certain page fails
/// </summary>
/// <param name="sender">The Frame which failed navigation</param>
/// <param name="e">Details about the navigation failure</param>
void App::OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e)
{
    throw ref new FailureException("Failed to load Page " + e->SourcePageType.Name);
}
#include<iostream>
#include<vector>
#define __STDC_CONSTANT_MACROS

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif
# include <stdint.h>
# include <inttypes.h>
extern"C" {

#include "C:/vcpkg/installed/x86-windows/include/libavutil/imgutils.h"
#include"C:/vcpkg/installed/x86-windows/include/libavcodec/avcodec.h"
#include"C:/vcpkg/installed/x86-windows/include/libavformat/avformat.h"
#include"C:/vcpkg/installed/x86-windows/include/libavutil/error.h"
#include"C:/vcpkg/installed/x86-windows/include/libavutil/mathematics.h"
#include "C:/vcpkg/installed/x86-windows/include/libavutil/samplefmt.h"
#include "C:/vcpkg/installed/x86-windows/include/libavutil/frame.h"
}
#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096
using namespace std;

class ffDecFile {
private:
	AVCodec * codec;
	AVCodecContext *c;
	int len;
	vector<uint8_t> inBuf;
	AVPacket * avpkt;
	FILE *f;
	int finished;
	AVFrame *decoded_frame;
	void cleanup() {
		if (this->c) {
			avcodec_close(this->c);
			av_free(this->c);
			av_free(this->decoded_frame);
		}
		if (this->f) {
			fclose(this->f);
		}
	}
	int decode() {
		/* Decode a part */
		if (this->avpkt->size > 0) {
			int got_packet = 0;
			if (!this->decoded_frame) {
				if (!(this->decoded_frame = av_frame_alloc())) {
					cout << "\n Failed to allocate memory ... Add more ram add more ram!";
					return -4;
				}
			}
			else
				av_frame_unref(this->decoded_frame);
			this->len = avcodec_decode_audio4(this->c, this->decoded_frame, &got_packet, this->avpkt);
			if (this->len < 0) {
				cout << "\n Error Reading packet";
				return -2;
			}
			this->avpkt->size -= len;
			this->avpkt->data += len;
			this->avpkt->pts = this->avpkt->dts = AV_NOPTS_VALUE;
			/* Prepare for net routine */
			if (this->avpkt->size < AUDIO_REFILL_THRESH) {
				memmove(this->inBuf.data(), this->avpkt->data, this->avpkt->size);
				this->avpkt->data = this->inBuf.data();
				this->len = fread(this->avpkt->data + this->avpkt->size, 1, AUDIO_INBUF_SIZE - this->avpkt->size, f);
				if (this->len > 0) {
					this->avpkt->size += len;
				}

			}
			return 0;
		}
		return 1; // If finished.
	}

public:
	int init(char *file, AVCodecID codec_enum) {
		/* Initialize defs */
		this->finished = 0;
		this->c = new AVCodecContext;
		this->len = 0;
		this->f = NULL;
		this->avpkt = new AVPacket;
		this->decoded_frame = new AVFrame;
		this->inBuf.resize(AUDIO_INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE);
		av_init_packet(this->avpkt);
		this->codec = avcodec_find_decoder(codec_enum);
		if (!this->codec) {
			cout << "\n Can't find the codec";
			this->cleanup();
			return -1;
		}
		this->c = avcodec_alloc_context3(this->codec);
		if (avcodec_open2(this->c, this->codec, NULL)< 0)
		{
			cout << "\n Error opening codec";
			this->cleanup();
			return -2;
		};
		f = fopen(file, "rb");
		if (!f) {
			cout << "\n Error opening file";
			this->cleanup();
			return -3;
		}
		/* Initial read */
		this->avpkt->size = fread(this->inBuf.data(), 1, AUDIO_INBUF_SIZE, f);
		this->avpkt->data = inBuf.data();
		return 0;
	}
	AVCodecContext * getContext() {
		return this->c;
	}
	AVFrame * getDecodedFrame() {
		int state = this->decode();
		if (!finished)
			if (state == 1 || state == 0) {
				if (state)
					this->finished = 1;
				return (this->decoded_frame);
			}
			else {
				this->cleanup();
			}
			return NULL;
	}
};

struct codecData {
	int bitRate;
	int sampleRate;
	int channels;
	enum AVSampleFormat sample_fmt;
};

class ffEnc {
	AVCodec *codec;
	AVCodecContext *c = NULL;
	AVFrame * frame;
	AVPacket * encoded_packet;
	int frame_size, i, j, out_size;
	void cleanup() {
		if (this->c) {
			avcodec_close(this->c);
			av_free(this->c);
		}
	}
	/*
	* Private function that actually encodes and returns the stuff.
	*/
	int encode() {
		int got_packet = 0;
		this->out_size = avcodec_encode_audio2(this->c, this->encoded_packet, this->frame, &got_packet);

		if (out_size < 0) {
			return -2; // known error in this case. Will be logged by Libavcodec.. hopefully wont mess with node.js
		}
		if (got_packet < 0) {
			cout << "Contact a Plumber!!!! \n we have got a leakage , and its unknown!!";
			return -4; // Unknown leakage get ur seals ready to fix this .
		}
		// Success
		return 0;
	}
public:
	/*
	* @Public Api
	* Constructor for the encoder class
	* and binding source for the node.js
	* though node.js will use different parameters
	* TODO: Change the parameters to v8 based JS objects that come from JavaScript.
	*/
	int init(char* fileName, AVCodecID codec_id, codecData defs) {
		this->codec = new AVCodec;
		this->frame = new AVFrame;

		this->encoded_packet = new AVPacket;
		this->encoded_packet->data = NULL;
		this->codec = avcodec_find_encoder(codec_id);
		if (!codec) {
			cout << "Error opening codec";
			this->cleanup();
			return -1;

		}
		this->c = avcodec_alloc_context3(this->codec);
		this->c->sample_fmt = defs.sample_fmt;
		this->c->bit_rate = defs.bitRate;
		this->c->channels = defs.channels;
		this->c->sample_rate = defs.sampleRate;

		/* Open the encoder */
		if (avcodec_open2(this->c, this->codec, NULL)< 0) {
			cout << "Error opening the codec";
			this->cleanup();
			return -2;
		}
		this->frame_size = this->c->frame_size;
		// Success.
		return 0;
	}
	/*
	* @Public API for encoding and also to be binded to node.js
	*/
	AVPacket * getEncodedPacket(AVFrame * toEncode) {
		// toEncode is the frame to encode . in node.js this will be ported to buffer to encode.
		this->frame = toEncode;
		int state = this->encode();
		if (state != 0) {
			return NULL;
		}
		else {
			return this->encoded_packet;
		}
	}
};



int main() {
	av_register_all();
	ffDecFile* ffmpegd = new ffDecFile();
	ffEnc * ffmpege = new ffEnc();
	FILE * outFile;
	AVPacket * out_pack = new AVPacket;
	outFile = fopen("ghoo.mp3", "wb+");
	cout << "\n Worked";
	if (ffmpegd->init("buu.mp3", AV_CODEC_ID_MP3) == 0) {
		cout << "\n File loaded";
	}
	codecData defs;

	AVFrame * outFrame = NULL;

	outFrame = ffmpegd->getDecodedFrame();

	if (outFrame) {
		AVCodecContext * dec_con = ffmpegd->getContext();
		defs.sample_fmt = dec_con->sample_fmt;// Findout whats our decoder is speaking
		defs.channels = dec_con->channels;  // now start making our encoder speak the same
		defs.sampleRate = dec_con->sample_rate; // and bing we are good to go.
		defs.bitRate = 64000;
		ffmpege->init("ghoo.mp3", AV_CODEC_ID_MP3, defs);
		out_pack = ffmpege->getEncodedPacket(outFrame);
		if (out_pack != NULL)
			fwrite(out_pack->data, 1, out_pack->size, outFile);
		else {
			fclose(outFile);
			cout << "No encoded frame , Call Plumber this is a major leak!";
			return -2;
		}
	}
	else {
		cout << "\n Unknown error occurred please consult Dr.Debugger.";
		return -1;
	}

	float * samples = NULL;

	long int total_samp = 0;
	while (outFrame != NULL) {
		/* Do some cool stuff with data  */
		outFrame = ffmpegd->getDecodedFrame();
		out_pack = ffmpege->getEncodedPacket(outFrame);
		if (out_pack != NULL)
			fwrite(out_pack->data, 1, out_pack->size, outFile);
		else {
			fclose(outFile);
			cout << "No encoded frame , Call Plumber this is a major leak!";
			return -2;
		}
	}
	cout << "\n Finished Reading file";
	cout << "\n File writing complete";
	return 0;
}