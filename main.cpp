#include "env.h"
#include "pjsua2.hpp"
#include <cstring>
#include <iostream>
#include <pj/os.h>
#include <pjmedia/format.h>
#include <pjsua-lib/pjsua.h>
#include <pjsua2/endpoint.hpp>
#include <pjsua2/media.hpp>
#include <pjsua2/types.hpp>

using namespace pj;

class PjAccount : public Account {
public:
  PjAccount() {}
  ~PjAccount() {}

  virtual void onRegState(OnRegStateParam &prm) {
    AccountInfo ai = getInfo();
    std::cout << (ai.regIsActive ? "*** Register: code="
                                 : "*** Unregister: code=")
              << prm.code << std::endl;
  }
};

class PjCall : public pj::Call {
public:
  PjCall(PjAccount &acc, int call_id = PJSUA_INVALID_ID) : Call(acc, call_id) {}

  pjsua_vid_win_id render_window = 0;

  void onCallState(pj::OnCallStateParam &prm) override {
    CallInfo ci = getInfo();
    if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
      delete this;
    }
  }

  virtual void onCallMediaState(pj::OnCallMediaStateParam &param) override {

    CallInfo ci = getInfo();

    AudioMedia *aud_med;
    VideoMedia *vid_med;
    AudDevManager &mgr = Endpoint::instance().audDevManager();

    for (unsigned i = 0; i < ci.media.size(); i++) {
      CallMediaInfo cmi = ci.media[i];
      if (cmi.type == PJMEDIA_TYPE_AUDIO &&
          cmi.status == PJSUA_CALL_MEDIA_ACTIVE) {
        aud_med = (AudioMedia *)getMedia(i);
      }

      else if (cmi.type == PJMEDIA_TYPE_VIDEO &&
               cmi.status == PJSUA_CALL_MEDIA_ACTIVE) {
        try {
          cmi.videoWindow.Show(true);
          break;
        } catch (const pj::Error &error) {
          std::cout << "\n\n Error: \n" << error.info() << "\n\n\n";
        }
      }
    }

    if (aud_med) {
      aud_med->startTransmit(mgr.getPlaybackDevMedia());
      mgr.getCaptureDevMedia().startTransmit(*aud_med);
    }
  }
};

class Sip_App : public pj::Endpoint {
public:
  void init() {
    EpConfig ep_cfg;
    ep_cfg.logConfig.level = 5;
    ep_cfg.medConfig.vidPreviewEnableNative = false;
    ep_cfg.medConfig.quality = 10;
    libCreate();
    libInit(ep_cfg);
    transportCreate(PJSIP_TRANSPORT_UDP, TransportConfig());
    libStart();
  }
  void deinit() { libDestroy(); }
};

void make_call(PjAccount *account) {
  CallOpParam prm(true);
  prm.opt.audioCount = 1;
  prm.opt.videoCount = 1;

  auto call = new PjCall(*account);

  std::string sip_address = "sip:";
  sip_address.append(DESTINY_NUMBER);
  sip_address.append("@");
  sip_address.append(HOST);

  call->makeCall(sip_address, prm); //example: sip:0002@127.1.1.1
}

void createAccount(PjAccount *acc) {
  AccountConfig accc_cfg;
  std::string sip_address = "sip:";
  sip_address.append(SIP_NUMBER);
  sip_address.append("@");
  sip_address.append(HOST);
  accc_cfg.idUri = sip_address; //example: sip:0001@127.1.1.1

  std::string sip_host = "sip:";
  sip_host.append(HOST);

  accc_cfg.regConfig.registrarUri = sip_host;  // example: sip:127.1.1.1
  accc_cfg.sipConfig.authCreds.push_back(
      AuthCredInfo("digerst", "*", SIP_NUMBER, 0, PASSWORD));
  accc_cfg.natConfig.iceEnabled = true;
  accc_cfg.mediaConfig.srtpUse = PJMEDIA_SRTP_OPTIONAL;
  accc_cfg.mediaConfig.srtpSecureSignaling = 0;
  accc_cfg.videoConfig.autoTransmitOutgoing = true;
  accc_cfg.videoConfig.defaultCaptureDevice = CAM_ID;
  acc->create(accc_cfg);
}

int main() {

  auto endpoint = new Sip_App();

  endpoint->init();

  PjAccount *account = new PjAccount;

  try {

    createAccount(account);

    make_call(account);
    pj_thread_sleep(200000000);
  } catch (const Error &e) {
    std::cout << e.info() << std::endl;
  }
}
