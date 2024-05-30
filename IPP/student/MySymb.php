<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

class MySymb extends Arg {
    protected mixed $data;

    public function __construct(mixed $data) {
        $this->data = $data;
    }

    public function getData(): mixed {
        return $this->data;
    }
}