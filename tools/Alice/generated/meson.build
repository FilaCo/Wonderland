antlr4_rt_dep = antlr4_rt.dependency('antlr4_static')

alice_antlr4_gen_files = custom_target(
    'antlr4-gen',
    input: alice_grammar_files,
    output: [
        'AliceLexer.cpp',
        'AliceLexer.h',
        'AliceParser.cpp',
        'AliceParser.h',
        'AliceListener.cpp',
        'AliceListener.h',
        'AliceBaseListener.cpp',
        'AliceBaseListener.h',
        'AliceVisitor.cpp',
        'AliceVisitor.h',
        'AliceBaseVisitor.cpp',
        'AliceBaseVisitor.h',
    ],
    command: [
        find_program('antlr4'),
        '-Dlanguage=Cpp',
        '-visitor',
        '-o', '@OUTDIR@',
        '-Xexact-output-dir',
        '@INPUT@',
    ],
)

alice_antlr4_gen_inc = [include_directories('.')]
alice_antlr4_gen_deps = [antlr4_rt_dep]

alice_antlr4_gen_lib = static_library(
    'AliceANTLR4Gen',
    alice_antlr4_gen_files,
    dependencies: alice_antlr4_gen_deps,
    include_directories: alice_antlr4_gen_inc,
)

alice_antlr4_gen_dep = declare_dependency(
    link_with: alice_antlr4_gen_lib,
    dependencies: alice_antlr4_gen_deps,
    sources: alice_antlr4_gen_files
)
