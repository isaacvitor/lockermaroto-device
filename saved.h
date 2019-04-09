const char SAVED_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Device Config</title>
    <style>
      .config-page {
        width: 360px;
        padding: 8% 0 0;
        margin: auto;
      }
      .form {
        position: relative;
        z-index: 1;
        background: #fff;
        max-width: 360px;
        margin: 0 auto 100px;
        padding: 45px;
        text-align: center;
        box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.2), 0 5px 5px 0 rgba(0, 0, 0, 0.24);
      }

      .form .message {
        margin: 15px 0 0;
        color: #b3b3b3;
        font-size: 12px;
      }
      a,
      .form .message a {
        color: #4caf50;
        text-decoration: none;
      }
      .form .config-form {
        display: block;
      }
      .container {
        position: relative;
        z-index: 1;
        max-width: 300px;
        margin: 0 auto;
      }
      .container:after,
      .container:before {
        content: '';
        display: block;
        clear: both;
      }
      .container .info {
        margin: 50px auto;
        text-align: center;
      }
      .container .info h1 {
        margin: 0 0 15px;
        padding: 0;
        font-size: 36px;
        font-weight: 300;
        color: #1a1a1a;
      }
      .container .info span {
        color: #4d4d4d;
        font-size: 12px;
      }
      .container .info span a {
        color: #000;
        text-decoration: none;
      }
      .container .info span .fa {
        color: #ef3b3a;
      }
      body {
        background: #76b852;
        background: -webkit-linear-gradient(right, #76b852, #8dc26f);
        background: -moz-linear-gradient(right, #76b852, #8dc26f);
        background: -o-linear-gradient(right, #76b852, #8dc26f);
        background: linear-gradient(to left, #76b852, #8dc26f);
        font-family: Roboto, sans-serif;
        -webkit-font-smoothing: antialiased;
        -moz-osx-font-smoothing: grayscale;
      }
    </style>
  </head>
  <body>
    <div class="config-page">
      <div class="form">
        <h1>Configurations</h1>
        <h2>OK</h2>
        <a href="/reset">Reset Device</a>
        <p class="message">
          Device Config - <a href="https://github.com/isaacvitor">Isaac Vitor</a>
        </p>
      </div>
    </div>
  </body>
</html>
)=====";