#ifndef COMMAND_COMMAND_H_
#define COMMAND_COMMAND_H_
#include "command_helper.h"
#include<memory>
#include<string>

// Command class supports commmand line api including
// Supports
// register a user through given username
// post or reply to chirps
// let one user follow another user
// read the chirp thread from given chirp id
// monitor the chirp on those people given user followed

class CommandClient {
 public:
  explicit CommandClient(std::shared_ptr<Channel> channel)
                        : stub_(ServiceLayer::NewStub(channel)) {}
  // Register a user through given username
  // If user already registered, return StatusCode::ALREADY_EXISTS
  Status RegisterUser(const string& registeruser);
  // Post a chirp based on username, chirp text, and its parent id
  // Return the posted chirp
  Chirp ChirpPost(const string& username, const string& chirp,
                  const string& reply);
  // follow allow user to follow another one
  // return a status whether follow is successful
  auto Follow(const string& username, const string& to_follow);
  // read a chirp thread given a chirp id
  // return a chirp array
  auto Read(const string& id);
  // monitor the chirp on those people given user followed
  void Monitor(const string& username);

 private:
  std::unique_ptr<ServiceLayer::Stub> stub_;
};
#endif  // COMMAND_COMMAND_H_
