--TEST--
IntlCalendar::get/getActualMaximum/getActualMinimum(): bad arguments (procedural)
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

function eh($errno, $errstr) {
echo "error: $errno, $errstr\n";
}
set_error_handler('eh');

try {
	var_dump(intlcal_get($c));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump(intlcal_get_actual_maximum($c));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump(intlcal_get_actual_minimum($c));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}

try {
	var_dump(intlcal_get($c, -1));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump(intlcal_get_actual_maximum($c, -1));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump(intlcal_get_actual_minimum($c, -1));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}

try {
	var_dump(intlcal_get($c, "s"));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump(intlcal_get_actual_maximum($c, "s"));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump(intlcal_get_actual_minimum($c, "s"));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}

try {
	var_dump(intlcal_get(1));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump(intlcal_get_actual_maximum(1));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
	var_dump(intlcal_get_actual_minimum(1));
} catch (Error $ex) {
	echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
--EXPECT--
error: 2, intlcal_get() expects exactly 2 parameters, 1 given
error: 2, intlcal_get(): intlcal_get: bad arguments
bool(false)
error: 2, intlcal_get_actual_maximum() expects exactly 2 parameters, 1 given
error: 2, intlcal_get_actual_maximum(): intlcal_get_actual_maximum: bad arguments
bool(false)
error: 2, intlcal_get_actual_minimum() expects exactly 2 parameters, 1 given
error: 2, intlcal_get_actual_minimum(): intlcal_get_actual_minimum: bad arguments
bool(false)
error: 2, intlcal_get(): intlcal_get: invalid field
bool(false)
error: 2, intlcal_get_actual_maximum(): intlcal_get_actual_maximum: invalid field
bool(false)
error: 2, intlcal_get_actual_minimum(): intlcal_get_actual_minimum: invalid field
bool(false)
error: 2, intlcal_get() expects parameter 2 to be integer, string given
error: 2, intlcal_get(): intlcal_get: bad arguments
bool(false)
error: 2, intlcal_get_actual_maximum() expects parameter 2 to be integer, string given
error: 2, intlcal_get_actual_maximum(): intlcal_get_actual_maximum: bad arguments
bool(false)
error: 2, intlcal_get_actual_minimum() expects parameter 2 to be integer, string given
error: 2, intlcal_get_actual_minimum(): intlcal_get_actual_minimum: bad arguments
bool(false)
error: 0, Argument 1 passed to intlcal_get() must be an instance of IntlCalendar, integer given

error: 0, Argument 1 passed to intlcal_get_actual_maximum() must be an instance of IntlCalendar, integer given

error: 0, Argument 1 passed to intlcal_get_actual_minimum() must be an instance of IntlCalendar, integer given

