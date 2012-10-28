<?php
require_once (dirname(__FILE__).'/nusoap/nusoap.php');

class EcordiaKeywordAlternates {
	var $apiKey;
	/**
	 * @var nusoap_client
	 */
	var $client;
	var $results = null;
	var $requestHasBeenExecuted = false;
	var $useSsl = false;

	function EcordiaKeywordAlternates($apiKey,$useSsl = false) {
		$this->useSsl = $useSsl;
		$this->apiKey = $apiKey;

		$loc = ($this->useSsl ? 'https' : 'http') . '://vesta.ecordia.com/optimizer/v1/contentanalysis.svc' . '/' . ($this->useSsl ? 'ssl' : 'nonssl') . '/';
		$this->client = new nusoap_client($loc);
		$this->client->soap_defencoding = 'utf-8';
	}

	function GetAlternateKeywords($keyword) {
		$contents = '
		<GetKeywordAlternates xmlns="https://vesta.ecordia.com">
			<request xmlns:a="http://optimizer.ecordia.com/types/" xmlns:i="http://www.w3.org/2001/XMLSchema-instance">
				<a:ApiKey>'.$this->apiKey.'</a:ApiKey>
				<a:SeedTerm>'.$keyword.'</a:SeedTerm>
			</request>
		</GetKeywordAlternates>
		';
		$contents = $this->client->serializeEnvelope($contents);

		$endpoint = 'https://vesta.ecordia.com/IContentAnalysis/GetKeywordAlternates';
		$results = $this->client->send($contents, $endpoint, 0, 180);
		$this->results = $results;
		$this->requestHasBeenExecuted = true;
	}

	function getRawResults() {
		if (!$this->requestHasBeenExecuted) {
			return array();
		} else {
			return $this->results;
		}
	}

	function hasError() {
		return $this->requestHasBeenExecuted && (! empty($this->results['faultcode']) || ($error = $this->client->getError()) || ! empty($this->results['AlternateKeywordsResult']['Exception']['Message']));
	}

	function getError() {
		if (!$this->requestHasBeenExecuted) {
			return false;
		} elseif (!empty($this->results['faultcode'])) {
			return array('Message'=>$this->results['faultstring']['!'], 'Number'=>$this->results['faultcode']);
		} elseif ($this->client->getError()) {
			return array('Message'=>$this->client->getError(), 'Number'=>1);
		} else {
			return $this->results['GetAnalysisResult']['Exception'];
		}
	}

	function getErrorMessage() {
		$error = $this->getError();
		if (is_array($error)) {
			return $error['Message'];
		} else {
			return false;
		}
	}

	function getErrorType() {
		$error = $this->getError();
		if (is_array($error)) {
			return $error['Number'];
		} else {
			return false;
		}
	}
}
