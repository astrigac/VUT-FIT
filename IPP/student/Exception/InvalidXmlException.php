<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student\Exception;

use IPP\Core\Exception\IPPException;
use IPP\Core\ReturnCode;
use Throwable;

/**
 * Exception for invalid XML format errors
 */
class InvalidXmlException extends IPPException
{
    public function __construct(string $message = "Invalid XML format error", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::INVALID_XML_ERROR, $previous);
    }
}