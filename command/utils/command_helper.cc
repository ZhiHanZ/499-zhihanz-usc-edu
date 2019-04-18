#include "command_helper.h"
namespace command_helper {
// switch to different cases based on the information
uint32_t CommandHandler(const string& regist, const string& user,
                        const string& chirp, const string& reply,
                        const string& read, const string& follow,
                        const bool& monitor, const string& stream) {
  if (regist != "" && user == "" && chirp == "" && reply == "" && read == "" &&
      follow == "" && monitor == false) {
    return kREGIST;
  } else if (regist == "" && user == "" && chirp == "" && reply == "" &&
             read == "" && follow == "" && monitor == false) {
    return kNULL;
  } else if (regist == "" && user != "" && chirp != "" && reply == "" &&
             read == "" && follow == "" && monitor == false) {
    return kCHIRP;
  } else if (regist == "" && user != "" && chirp != "" && reply != "" &&
             read == "" && follow == "" && monitor == false) {
    return kREPLY;
  } else if (regist == "" && user == "" && chirp == "" && reply == "" &&
             read != "" && follow == "" && monitor == false) {
    return kREAD;
  } else if (regist == "" && user != "" && chirp == "" && follow == "" &&
             reply == "" && read == "" && monitor == true) {
    return kMONITOR;
  } else if (regist == "" && user != "" && chirp == "" && follow != "" &&
             reply == "" && read == "" && monitor == false) {
    return kFOLLOW;
  } else if (regist == "" && user !="" && chirp == "" && follow == "" &&
             reply == "" && read == "" && monitor == false && stream != "") {
  	return kSTREAM;
  } else {
    return kOTHERS;
  }
}
}  // namespace command_helper
namespace format {
// print formated chirp
void PrintChirp(const Chirp& chirp) {
  LOG(INFO) << "username: " << chirp.username();
  LOG(INFO) << "text: " << chirp.text();
  LOG(INFO) << "parent id:  " << chirp.parent_id();
  LOG(INFO) << "Chirp id: " << chirp.id();
  Timestamp time = chirp.timestamp();
  //  thread safety
  time_t sec = time.seconds();
  struct tm newtime;
  localtime_r(&sec, &newtime);
  char buffer[50];
  asctime_r(&newtime, buffer);
  LOG(INFO) << "posted time: " << buffer;
}
// print out help info
void HelpInfo() {
  LOG(INFO) << "If you want to regist: ./command -regist=<Your name>";
  LOG(INFO) << "If you want to chirp: ./command -user=<Your name> "
               "-chirp=<Your text>";
  LOG(INFO) << "If you want to reply to others: "
            << "./command -user =<Your name>"
            << " -chirp=<Your text>"
            << " -reply=<Reply ID>";
  LOG(INFO) << "If you want to follow somebody: "
            << "./command -user =<Your name>"
            << " -follow=<Other username>";
  LOG(INFO) << "If you want to monitor the followed people for one given user"
            << " ./command -user=<Your name> -monitor";
}
// Print out the status after regist
void RegistHandler(const Status& status, const string& regist) {
  if (!status.ok()) {
    LOG(ERROR) << status.error_message();
  } else {
    LOG(INFO) << "Hello " << regist;
  }
}
// Print out the status after follow
void FollowHandler(const Status& status, const string& follow) {
  if (!status.ok()) {
    LOG(ERROR) << status.error_message();
  } else {
    LOG(INFO) << "successfully followed " << follow;
  }
}
}  // namespace format
