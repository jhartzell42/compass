needle 2007;

"package compass::channel::buffered;"
private class $ChannelBuffer
    !rawChannel,
    !tokens,
;

public class $BufferedInputChannel
    !channelBuffer,
    !position,
;
$BufferedInputChannel mixin $ChannelWrapper;
$BufferedInputChannel mixin $InputChannel;

$ChannelBuffer constructor newFromChannel: !rawChannel {
    !tokens := []
}

public $BufferedInputChannel class newFromChannel: @channel {
    ^newWithChannelBuffer: ($ChannelBuffer newFromChannel: @channel)
	position: 0;
}

$BufferedInputChannel class (@args) {
    ^newFromChannel: @args[0]
}

private $BufferedInputChannel constructor
    newWithChannelBuffer: !channelBuffer
	position: !position;

$ChannelBuffer [ @pos ] {
    "TODO: synchronization semantics"
    {
	{
	    ^!tokens[@pos];
	} onIndexOutOfBounds: {
	    !tokens add: !rawChannel next;
	}
    } repeat
}

private $BufferedInputChannel assertMatchingChannelBuffer: @buf (7,ro) {
    $InvalidArgumentException assert: @buf == !channelBuffer
}

$BufferedInputChannel next {
    ^!channelBuffer[!position];
    !position := !position + 1
}

public $BufferedInputChannel forkedChannel {
    ^$BufferedInputChannel newWithChannelBuffer: !channelBuffer
			    position: !position;
}

private $BufferedInputChannel position (7,ro) {
    ^position
}

public $BufferedInputChannel catchUpWith: @other {
    assertMatchingChannelBuffer: !channelBuffer;
    !position = @other position;
}

public exception $ResetChannel #onResetChannel:;

public $BufferedInputChannel try: @blk {
    @attemptChannel := forkedChannel;
    {
	@blk(@attemptChannel);
	catchUpWith: @attemptChannel
    } onResetChannel: { "attempt failed - don't go anywhere" }
}
