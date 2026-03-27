use crate::source::Spanned;

#[derive(Clone, Debug)]
pub struct AliceScript<'src> {
    pub top_level_objects: Vec<Spanned<TopLevelObject<'src>>>,
}

#[derive(Clone, Debug)]
pub enum TopLevelObject<'src> {
    TopLevelStmt(TopLevelStmtKind<'src>),
    TopLevelDecl,
}

#[derive(Clone, Debug)]
pub enum TopLevelStmtKind<'src> {
    UsingNamespace { namespace_ident: &'src str },
}

// #[derive(Clone, Debug)]
// pub enum ExprKind<'src> {
//     Binary {
//         lhs: Box<Spanned<Self>>,
//         op: Token<'src>,
//         rhs: Box<Spanned<Self>>,
//     },
//     Unary {
//         op: Token<'src>,
//         rhs: Box<Spanned<Self>>,
//     },
//     Grouped {
//         expr: Box<Spanned<Self>>,
//     },
//     Lit {
//         data: Spanned<LitData<'src>>,
//     },
// }

// impl<'src> ExprKind<'src> {}

// #[derive(Clone, Copy, Debug, PartialEq, Eq)]
// pub struct LitData<'src> {
//     pub kind: LitKind,
//     pub symbol: &'src str,
// }

// impl<'src> LitData<'src> {
//     pub const fn new(kind: LitKind, symbol: &'src str) -> Self {
//         Self { kind, symbol }
//     }
//     pub const fn bin_lit(symbol: &'src str) -> Self {
//         Self::new(Int { base: Bin }, symbol)
//     }
//     pub const fn oct_lit(symbol: &'src str) -> Self {
//         Self::new(Int { base: Oct }, symbol)
//     }
//     pub const fn dec_int_lit(symbol: &'src str) -> Self {
//         Self::new(Int { base: Dec }, symbol)
//     }
//     pub const fn hex_int_lit(symbol: &'src str) -> Self {
//         Self::new(Int { base: Hex }, symbol)
//     }
// }
