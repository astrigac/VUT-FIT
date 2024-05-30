<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

class MyNil extends MyConst {
    public function __construct() {
        parent::__construct('nil');
    }
}