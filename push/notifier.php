<?php

    require __DIR__ . '/vendor/autoload.php';

    use Minishlink\WebPush\WebPush;
    use Minishlink\WebPush\Subscription;

    $auth = [
        'VAPID' => [
            'subject' => 'mailto:info@phlhg.ch',
            'publicKey' => 'BJq9JHonZAGTS_IYH0sfuEK0JNLJ576r7NOATOxaYvEXQgVJJjc3rhc-dkP-05YA_4Esc2X55DM21F-c-y4zY-w', 
            'privateKey' => 'qY6UuBUOHjytyjmtObHNhmyQXPwfFrolMRRh5eKLZo8',
        ],
    ];

    $invalid_endpoints = [];

    function output($message){
        fwrite(STDOUT, $message."\n");
    }

    function error($message){
        fwrite(STDERR, $message."\n");
        exit(2);
    }

    if(!isset($argv[1])){
        error("Missing argument for message file");
    }

    $in = $argv[1];
    $out = isset($argv[2]) ? $argv[2] : "output.txt";

    if(!file_exists($in)){
        error("Give message file does not exist");
    }

    $content = file_get_contents($in);

    if($content === false){
        error("Couldn't read message file");
    }

    $messages = json_decode($content);

    if($messages == null){
        error("Couldn't parse message file");
    }

    $webPush = new WebPush($auth);
    $webPush->setReuseVAPIDHeaders(true);
    $webPush->setDefaultOptions([
        'TTL' => 300,
        'batchSize' => 200
    ]);

    foreach($messages as $message){

        $subscription = Subscription::create((array) [
            "endpoint" => $message->subscription->endpoint,
            "publicKey" => $message->subscription->keys->p256dh,
            "authToken" => $message->subscription->keys->auth
        ]);

        try{

            $report = $webPush->sendOneNotification(
                $subscription,
                json_encode(array(
                    "type" => $message->type,
                    "data" => $message->data
                ))
            );

            if(!$report->isSuccess()){
                array_push($invalid_endpoints,$report->getEndpoint());
            }

        } catch(Error $e){

            array_push($invalid_endpoints,$message->subscription->endpoint);

        }

    }

    file_put_contents($out, implode("\n", $invalid_endpoints));


?>