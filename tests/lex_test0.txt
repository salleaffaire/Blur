// Test string literals

// Positive Tests
// --------------------------------------------
var a1 = "\\\"\\";
var a2 = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
var a3 = "!@#$%^&*(){}[]|\\:;?/<>,.~`";
var a4 = "";
var a5 = "11 \t\t 11";
var a6 = "11\n11";
var a7 = "11\r22";

// Negative Tests
// --------------------------------------------
var a8 = "111";
