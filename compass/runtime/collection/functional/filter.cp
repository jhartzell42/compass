needle 2007;

"TODO doc"
"package compass::collection::functional::filter;"

private $true thenAdd: @item to: @acc {
    ^@acc adding: @item
}
private $false thenAdd: _ to: @acc {
    ^@acc
}
public $Collection filter: @filter {
    "TODO Filter type when it is before 1am @filter := @filter asFilter;"
    ^reduce: { @acc, @item --
	@filter(@item) thenAdd: @item to: @acc
    } withIdentity: species new
}
public $Collection alias #|-> #filter:;
