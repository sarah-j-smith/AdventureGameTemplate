#pragma once

#include <string>
#include <optional>

#include "nlohmann/json.hpp"

#include "Test.h"

#define PROTOCOL_VERSION "0.1.0"

/** https://github.com/nlohmann/json  */
using json = nlohmann::json;

class Event
{
public:
	std::string Serialize() const;

	/** 
	 * Populate the event from the json data. 
	 * @param Data json to add
	 * @throws exception if the json accesses fail exceptionally
	 */
	virtual void AddData(const json &Data) = 0;

protected:
	// The type of the event.
	//
	// This is always one of the subclass types listed below.
	std::string type;

	// The time (in milliseconds) that has elapsed since the test runner started.
	int time;

	virtual std::optional<std::string> JsonLine() const = 0;

	virtual json ToJson() const;
};



///////////////////////////////////
///
///  START EVENT
///

class StartEvent : public Event
{
public:
	virtual void AddData(const json &Data) override;

protected:

	std::string type = "start";

	// The version of the JSON reporter protocol being used.
	//
	// This is a semantic version, but it reflects only the version of the
	// protocol—it's not identical to the version of the test runner itself.
	std::string protocolVersion = PROTOCOL_VERSION;

	// The version of the test runner being used.
	//
	// This is null if for some reason the version couldn't be loaded.
	std::optional<std::string> runnerVersion;

	// The pid of the VM process running the tests.
	std::optional<int> pid;

	virtual json ToJson() const override;
};



///////////////////////////////////
///
///  ALL SUITES EVENT
///

class AllSuitesEvent : public Event
{
public:
	virtual void AddData(const json &Data) override;

protected:

	std::string type = "allSuites";

	/// The total number of suites that will be loaded.
	int count;

	virtual json ToJson() const override;
};



///////////////////////////////////
///
///  SUITE EVENT
///

class SuiteEvent : public Event
{
public:
	virtual void AddData(const json &Data) override;

protected:

	std::string type = "suite";

	/// Metadata about the suite.
	std::string suite;

	virtual json ToJson() const override;
};



///////////////////////////////////
///
///  DEBUG EVENT
///

class DebugEvent : public Event
{
public:
	virtual void AddData(const json &Data) override;

protected:

	std::string type = "debug";

	/// The suite for which debug information is reported.
	int suiteID;

	/// The HTTP URL for the Dart Observatory, or `null` if the Observatory isn't
	/// available for this suite.
	std::optional<std::string> observatory;

	/// The HTTP URL for the remote debugger for this suite's host page, or `null`
	/// if no remote debugger is available for this suite.
	std::optional<std::string> remoteDebugger;

	virtual json ToJson() const override;
};




///////////////////////////////////
///
///  GROUP EVENT
///

class GroupEvent : public Event
{
public:
	virtual void AddData(const json &Data) override;

protected:

	std::string type = "group";

	/// Metadata about the group.
	std::string group;

	virtual json ToJson() const override;
};




///////////////////////////////////
///
///  TEST START EVENT
///

class TestStartEvent : public Event
{
public:
	virtual void AddData(const json &Data) override;

protected:

	std::string type = "testStart";

	// Metadata about the test that started.
	Test test;

	virtual json ToJson() const override;
};




///////////////////////////////////
///
///  MESSAGE EVENT
///

class MessageEvent : public Event
{
public:
	virtual void AddData(const json &Data) override;

protected:

	std::string type = "print";

	// The ID of the test that printed a message.
	int testID;

	// The type of message being printed.
	std::string messageType;

	// The message that was printed.
	std::string message;

	virtual json ToJson() const override;
};




///////////////////////////////////
///
///  ERROR EVENT
///

class ErrorEvent : public Event
{
public:
	virtual void AddData(const json &Data) override;

protected:

	std::string type = "error";

	// The ID of the test that experienced the error.
	int testID;

	// The result of calling toString() on the error object.
	std::string error;

	// The error's stack trace, in the stack_trace package format.
	std::string stackTrace;

	// Whether the error was a TestFailure.
	bool isFailure;

	virtual json ToJson() const override;
};




///////////////////////////////////
///
///  TEST DONE EVENT
///

class TestDoneEvent : public Event
{
public:
	virtual void AddData(const json &Data) override;

protected:

	std::string type = "testDone";

	// The ID of the test that completed.
	int testID;

	/**
	 *  The result of the test.
		  "success" if the test had no errors.
		  "failure" if the test had a TestFailure but no other errors.
		  "error" if the test had an error other than a TestFailure.
	  */
	std::string result;

	// Whether the test's result should be hidden.
	bool hidden;

	// Whether the test (or some part of it) was skipped.
	bool skipped;

	virtual json ToJson() const override;
};




///////////////////////////////////
///
///  DONE EVENT
///

enum class SuccessStatus : unsigned char
{
	unknown,
	success,
	failure,
};

class DoneEvent : public Event
{
public:
	virtual void AddData(const json &Data) override;

protected:

	std::string type = "done";

	// Whether all tests succeeded (or were skipped).
	//
	// Will be `null` if the test runner was closed before all tests completed
	// running.
	SuccessStatus success = SuccessStatus::unknown;

	virtual json ToJson() const override;
};
