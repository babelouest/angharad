const path = require('path');
const webpack = require('webpack');

module.exports = {
  entry: path.resolve(__dirname, './src/index.js'),
  module: {
    rules: [
      {
        test: /\.(js|jsx)$/,
        exclude: /node_modules/,
        use: ['babel-loader'],
      },
      {
        test: /\.css$/,
        use: [ 'style-loader', 'css-loader' ]
      }
    ],
  },
  resolve: {
    extensions: ['*', '.js', '.jsx'],
  },
  output: {
    path: path.resolve(__dirname, './dist'),
    filename: 'bundle.js',
  },
  plugins: [new webpack.HotModuleReplacementPlugin()],
  devServer: {
    static: path.resolve(__dirname, './dist'),
    hot: true,
    compress: true,
    port: 3000,
    host: 'localhost',
    open: true,
    proxy: {
      '/angharad': {
        target: 'http://localhost:2473/',
        secure: false,
        changeOrigin: true
      },
      '/benoic': {
        target: 'http://localhost:2473/',
        secure: false,
        changeOrigin: true
      },
      '/carleon': {
        target: 'http://localhost:2473/',
        secure: false,
        changeOrigin: true
      },
      '/.well-known/angharad-configuration': {
        target: 'http://localhost:2473/',
        secure: false,
        changeOrigin: true
      }
    }
  }
};
