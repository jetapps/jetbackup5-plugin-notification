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

use JetBackup\Core\Data\ArrayData;
use JetBackup\Core\Exception\ConnectionException;
use JetBackup\Core\Exception\HttpRequestException;
use JetBackup\Core\Exception\NotificationException;
use JetBackup\Core\Exception\ValidationException;
use JetBackup\Core\Notification\Integration\Notification;
use JetBackup\Core\Web\JetHttp;

defined( '__JETBACKUP__' ) or die( 'Restricted access' );


class HelloWorld implements Notification {

	private $_options;

	public function __construct() {
		$this->_options = new ArrayData();
	}

	private function setMyField(string $value): void { $this->_options->set('myfield', $value); }
	private function getMyField():string { return $this->_options->get('myfield'); }

	/**
	 * @param int|null $level 1 - Information, 2 - Warning, 4 - Critical and null for mixed levels
	 * @param string $subject
	 * @param string $message
	 * @param array $attachments
	 *
	 * @throws NotificationException
	 * @throws ConnectionException
	 */
	public function send(int|null $level, string $subject, string $message, array $attachments=[]):void {

		try {
			$response = JetHttp::request()
				->setMethod(JetHttp::METHOD_POST)
				->addHeader('Content-Type', 'application/json')
				->setBody([
					'myfield'       => $this->getMyField(),
					'action'        => 'sendMessage',
					'level'         => $level,
					'subject'       => $subject,
					'message'       => $message,
				])
				->exec('https://your-plugin-url.com');

		} catch(HttpRequestException $e) {
			throw new NotificationException($e->getMessage());
		}

		if($response->getHeaders()->getCode() != 200) throw new ConnectionException("API returned invalid status code ({$response->getHeaders()->getCode()})");
	}

	/**
	 * Validate all plugin fields - throw ValidationException on error
	 *
	 * @throws ValidationException
	 */
	public function validateFields():void {}

	/**
	 * @param object $data
	 */
	public function setData(object $data):void {
		// set fields one by one
		if(isset($data->myfield)) $this->setMyField($data->myfield);
		
		// OR set fields all at once
		$this->_options->setData((array) $data);
	}

	/**
	 * @return array
	 */
	public function getData(): array {
		return $this->_options->getData();
	}

	/**
	 * UnStringify the data and use it in the plugin 
	 * 
	 * @param string $data
	 */
	public function setSerializedData(string $data): void {
		$this->setData(json_decode($data));
	}

	/**
	 * Stringify the data - JB will save the data for you
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
	public function testConnection():void {

		try {
			$response = JetHttp::request()
				->setMethod(JetHttp::METHOD_POST)
				->addHeader('Content-Type', 'application/json')
				->setBody([
					'myfield'       => $this->getMyField(),
					'action'        => 'testConnection'
				])
				->exec('https://your-plugin-url.com');

		} catch(HttpRequestException $e) {
			throw new ConnectionException($e->getMessage());
		}

		if($response->getHeaders()->getCode() != 200) throw new ConnectionException("API returned invalid status code ({$response->getHeaders()->getCode()})");
	}

	/**
	 * @return array
	 */
	public function protectedFields(): array { 
		return ['myfield']; 
	}

	/**
	 * @return void
	 */
	public function routineTasks(): void {
	}
}