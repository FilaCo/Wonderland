cases = [
    {
        'name': 'test_vec',
        'sources': ['test_vec.c', unity_gen_runner.process('test_vec.c')],
    },
]

foreach case : cases
    test(
        case['name'],
        executable(
            case['name'],
            case['sources'],
            dependencies: unity_dep,
            include_directories: stdx_inc,
            link_with: stdx_static,
        ),
    )
endforeach
