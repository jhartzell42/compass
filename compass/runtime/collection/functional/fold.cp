needle 2007;

"TODO doc"
"package compass::collection::functional::fold;"

private singleton $reductionNotStarted;
private $reductionNotStarted foldItem: @item
				using: @blk
				filteringInitialWith: @blk2 {
    ^@blk2(@item)
}
$Object foldItem: @item using: @blk filteringInitialWith: _{
    ^@blk(self, @item)
}

public $Collection singleReduce: @blk withIdentityFilter: @blk2 {
    ^reduce: { @reduction, @item --
	@blah := @reduction foldItem: @item using: @blk filteringInitialWith: @blk2
    } withIdentity: $reductionNotStarted
}

public $Collection singleReduce: @blk {
    ^singleReduce: @blk withIdentityFilter: #self
}

public $Collection alias #>< #reduce:;
public $Collection alias #singleReduce: #reduce:;

public $Collection reduce: @blk
		    withIdentity: @reduction
		    withLoopBlock: @looper {
    @looper(self,{ @item --
	@reduction := @blk(@reduction, @item)
    });
    ^@reduction
}

public $Collection reduce: @blk withIdentity: @reduction {
    "TODO: do this using ``reflection''"
    ^reduce: @blk withIdentity: @reduction withLoopBlock: #serialLoop:;
}
