<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

class MyVar extends MySymb {
    private string $frame;
    private string $name;

    public function __construct(string $frame, string $name, MyConst $data) {
        parent::__construct($data);
        $this->frame = $frame;
        $this->name = $name;
    }

    public function getFrame(): string {
        return $this->frame;
    }

    public function getName(): string {
        return $this->name;
    }
}
