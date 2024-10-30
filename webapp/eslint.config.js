/* eslint-env node */
import js from "@eslint/js";
import vueTsEslintConfig from '@vue/eslint-config-typescript';
import pluginVue from 'eslint-plugin-vue';

export default [
    js.configs.recommended,
    ...pluginVue.configs['flat/essential'],
    ...vueTsEslintConfig(),
    {
        files: [
            '**/*.vue',
            '**/*.js',
            '**/*.jsx',
            '**/*.cjs',
            '**/*.mjs',
            '**/*.ts',
            '**/*.tsx',
            '**/*.cts',
            '**/*.mts',
        ],
        languageOptions: {
            ecmaVersion: 2022,
        },
    },
];
