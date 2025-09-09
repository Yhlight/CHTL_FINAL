use crate::scanner::unified_scanner::UnifiedScanner;

#[test]
fn test_scan_for_scripts() {
    let input = r#"
        div {
            script {
                const a = 1;
                {{.box}}->listen({ click: () => { console.log("hello"); } });
            }
        }
        p {
            text { "some text" }
        }
        script {
            const b = 2;
        }
    "#;

    let expected_scripts = vec![
        r#"
                const a = 1;
                {{.box}}->listen({ click: () => { console.log("hello"); } });
            "#.to_string(),
        r#"
            const b = 2;
        "#.to_string(),
    ];

    let mut scanner = UnifiedScanner::new(input);
    let extracted_scripts = scanner.scan_for_scripts();

    assert_eq!(extracted_scripts.len(), expected_scripts.len());
    for (i, expected) in expected_scripts.iter().enumerate() {
        assert_eq!(extracted_scripts[i].trim(), expected.trim());
    }
}
