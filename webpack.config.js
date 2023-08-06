const { resolve: pathResolve } = require("path");
const { readdirSync } = require("fs-extra")
const HtmlWebpackPlugin = require('html-webpack-plugin')


// Code to add multiple pages taken from:
// https://dev.to/marcinwosinek/tutorial-for-building-multipage-website-with-webpack-4gdk
// Todo tutti questi path andrebbero cambiati in modo da avere path assoluto e da usare pathResolve.
// Output folder path is inside auto generated dist folder
const htmlPage = new HtmlWebpackPlugin({
      inject: true,
      template: `./code/display/index.ejs`, // Input ejs template file location
      filename: `./html/index.html`, // Output html file name
    })

module.exports = {
  entry: "./code/display/index.js",

  // Add other plugins in the array here.
  plugins: [htmlPage],

  output: {
    path: pathResolve(__dirname, 'dist'),
    filename: './js/index.js',
    clean: true,
  },

  // Following tutorial from: https://dev.to/marcinwosinek/tutorial-for-building-multipage-website-with-webpack-4gdk
  optimization: {
    splitChunks: {
      chunks: "all",
    },
  },

  mode: "development",

  module: {
    rules: [
      {
        test: /\.css$/,
        use: [
          'style-loader',
          'css-loader'
        ]
      }
    ]
  }
}
