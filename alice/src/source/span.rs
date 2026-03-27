#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Span {
    pub lo: u32,
    pub hi: u32,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Spanned<T> {
    pub value: T,
    pub span: Span,
}
