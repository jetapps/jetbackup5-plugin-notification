<?php
/*
*
* JetBackup @ package
* Created By Idan Ben-Ezra
*
* Copyrights @ JetApps
* https://www.jetapps.com
*
**/
namespace JetBackup\Core\Notification\HelloWorld;

use GuzzleHttp\Client;
use JetBackup\Core\Data\ArrayData;
use JetBackup\Core\Exception\ConnectionException;
use JetBackup\Core\Exception\NotificationException;
use JetBackup\Core\Exception\ValidationException;
use JetBackup\Core\Notification\Integration\Notification;

defined( '__JETBACKUP__' ) or die( 'Restricted access' );


class HelloWorld implements Notification {

	private $_options;

	public function __construct() {
		$this->_options = new ArrayData();
	}

	private function setMyField($value) { $this->_options->set('myfield', $value); }
	private function getMyField() { return $this->_options->get('myfield'); }

	/**
	 * @param int|null $level 1 - Information, 2 - Warning, 4 - Critical and null for mixed levels
	 * @param string $subject
	 * @param string $message
	 * @param array $attachments
	 *
	 * @throws NotificationException
	 */
	public function send($level, $subject, $message, $attachments=[]) {

		$client = new Client();
		$response = $client->request("POST", 'https://your-plugin-url.com', [
			'form_params'   => [
				'myfield'       => $this->getMyField(),
				'action'        => 'sendMessage',
				'level'         => $level,
				'subject'       => $subject,
				'message'       => $message,
			],
			'headers'       => [
				'Content-type: application/json',
			],
		]);

		if($response->getStatusCode() != 200) throw new NotificationException("API returned invalid status code ({$response->getStatusCode()})");
	}

	/**
	 * Validate all plugin fields - throw ValidationException on error
	 *
	 * @throws ValidationException
	 */
	public function validateFields() {}

	/**
	 * @param object $data
	 */
	public function setData($data) {
		// set fields one by one
		if(isset($data->myfield)) $this->setMyField($data->myfield);
		
		// OR set fields all at once
		$this->_options->setData((array) $data);
	}

	/**
	 * @return array
	 */
	public function getData(): array {
		return (array) $this->_options->getData();
	}

	/**
	 * Stringify the data - JB will save the data for you 
	 * 
	 * @param string $data
	 */
	public function setSerializedData($data) {
		$this->setData(json_decode($data));
	}

	/**
	 * UnStringify the data
	 * 
	 * @return string
	 */
	public function getSerializedData(): string {
		return json_encode((object) $this->getData());
	}

	/**
	 * Check plugin connection - throw ConnectionException on error
	 * 
	 * @throws ConnectionException
	 */
	public function testConnection() {

		$client = new Client();
		$response = $client->request("POST", 'https://your-plugin-url.com', [
			'form_params'   => [
				'myfield'       => $this->getMyField(),
				'action'        => 'testConnection'
			],
			'headers'       => [ 
				'Content-type: application/json', 
			],
		]);

		if($response->getStatusCode() != 200) throw new ConnectionException("API returned invalid status code ({$response->getStatusCode()})");

	}

	/**
	 * @return array
	 */
	public function protectedFields(): array { 
		return ['myfield']; 
	}
}