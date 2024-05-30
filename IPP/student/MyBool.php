<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

class MyBool extends MyConst {
    public function __construct(bool $data) {
        parent::__construct($data);
    }
}