module("luci.controller.watsond_controller", package.seeall)

function index()
	entry({"admin", "services", "ibm_watson_iot"}, cbi("watsond_model"), ("IBM Watson IoT Platform"),204)
end
