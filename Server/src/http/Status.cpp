#include "Status.h"

const char* statusCodeToStr(int code)
{
	switch (code) {
	case StatusCode::CONTINUE: return "CONTINUE";
	case StatusCode::SWITCHING_PROTOCOL: return "SWITCHING PROTOCOL";
	case StatusCode::PROCESSING: return "PROCESSING";
	case StatusCode::OK: return "OK";
	case StatusCode::CREATED: return "CREATED";
	case StatusCode::ACCEPTED: return "ACCEPTED";
	case StatusCode::NON_AUTHORITATIVE_INFORMATION: return "NON AUTHORITATIVE INFORMATION";
	case StatusCode::NO_CONTENT: return "NO CONTENT";
	case StatusCode::RESET_CONTENT: return "RESET CONTENT";
	case StatusCode::PARTIAL_CONTENT: return "PARTIAL CONTENT";
	case StatusCode::MULTI_STATUS: return "MULTI STATUS";
	case StatusCode::IM_USED: return "IM USED";
	case StatusCode::MULTIPLE_CHOICE: return "MULTIPLE CHOICE";
	case StatusCode::MOVED_PERMANENTLY: return "MOVED PERMANENTLY";
	case StatusCode::FOUND: return "FOUND";
	case StatusCode::SEE_OTHER: return "SEE OTHER";
	case StatusCode::NOT_MODIFIED: return "NOT MODIFIED";
	case StatusCode::USE_PROXY: return "USE PROXY";
	case StatusCode::UNUSED: return "UNUSED";
	case StatusCode::TEMPORARY_REDIRECT: return "TEMPORARY REDIRECT";
	case StatusCode::PERMANENT_REDIRECT: return "PERMANENT REDIRECT";
	case StatusCode::BAD_REQUEST: return "BAD REQUEST";
	case StatusCode::UNAUTHORIZED: return "UNAUTHORIZED";
	case StatusCode::PAYMENT_REQUIRED: return "PAYMENT REQUIRED";
	case StatusCode::FORBIDDEN: return "FORBIDDEN";
	case StatusCode::NOT_FOUND: return "NOT FOUND";
	case StatusCode::METHOD_NOT_ALLOWED: return "METHOD NOT ALLOWED";
	case StatusCode::NOT_ACCEPTABLE: return "NOT ACCEPTABLE";
	case StatusCode::PROXY_AUTHENTICATION_REQUIRED: return "PROXY AUTHENTICATION REQUIRED";
	case StatusCode::REQUEST_TIMEOUT: return "REQUEST TIMEOUT";
	case StatusCode::CONFLICT: return "CONFLICT";
	case StatusCode::GONE: return "GONE";
	case StatusCode::LENGTH_REQUIRED: return "LENGTH REQUIRED";
	case StatusCode::PRECONDITION_FAILED: return "PRECONDITION FAILED";
	case StatusCode::PAYLOAD_TOO_LARGE: return "PAYLOAD TOO_LARGE";
	case StatusCode::URI_TOO_LONG: return "URI_TOO_LONG";
	case StatusCode::UNSUPPORTED_MEDIA_TYPE: return "UNSUPPORTED MEDIA TYPE";
	case StatusCode::REQUEST_RANGE_NOT_SATISFIABLE: return "REQUEST RANGE_NOT SATISFIABLE";
	case StatusCode::EXPECTATION_FAILED: return "EXPECTATION FAILED";
	case StatusCode::IM_A_TEAPOT: return "IM A TEAPOT";
	case StatusCode::MISDIRECTED_REQUEST: return "MISDIRECTED REQUEST";
	case StatusCode::UNPROCESSABLE_ENTITY: return "UNPROCESSABLE ENTITY";
	case StatusCode::LOCKED: return "LOCKED";
	case StatusCode::FAILED_DEPENDENCY: return "FAILED DEPENDENCY";
	case StatusCode::UPGRADE_REQUIRED: return "UPGRADE REQUIRED";
	case StatusCode::PRECONDITION_REQUIRED: return "PRECONDITION REQUIRED";
	case StatusCode::TOO_MANY_REQUEST: return "TOO MANY REQUEST";
	case StatusCode::REQUEST_HEADER_FIELDS_TOO_LARGE: return "REQUEST HEADER FIELDS TOO_LARGE";
	case StatusCode::UNAVAILABLE_FOR_LEGAL_REASONS: return "UNAVAILABLE FOR_LEGAL REASONS";
	case StatusCode::INTERNAL_SERVER_ERROR: return "INTERNAL SERVER ERROR";
	case StatusCode::NOT_IMPLEMENTED: return "NOT IMPLEMENTED";
	case StatusCode::BAD_GATEWAY: return "BAD GATEWAY";
	case StatusCode::SERVICE_UNAVAILABLE: return "SERVICE UNAVAILABLE";
	case StatusCode::GATEWAY_TIMEOUT: return "GATEWAY TIMEOUT";
	case StatusCode::HTTP_VERSION_NOT_SUPPORTED: return "HTTP VERSION NOT SUPPORTED";
	case StatusCode::VARIANT_ALSO_NEGOCIATE: return "VARIANT ALSO NEGOCIATE";
	case StatusCode::INSUFFICIENT_STORAGE: return "INSUFFICIENT STORAGE";
	case StatusCode::LOOP_DETECTED: return "LOOP DETECTED";
	case StatusCode::NOT_EXTENDED: return "NOT EXTENDED";
	case StatusCode::NETWORK_AUTHENTICATION_REQUIRED: return "NETWORK AUTHENTICATION REQUIRED";
	}
}
