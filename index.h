const char INDEX_page[] PROGMEM = R"=====(
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
      .form input {
        font-family: Roboto, sans-serif;
        outline: 0;
        background: #f2f2f2;
        width: 100%;
        border: 0;
        margin: 0 0 15px;
        padding: 15px;
        box-sizing: border-box;
        font-size: 14px;
      }
      .form button {
        font-family: Roboto, sans-serif;
        text-transform: uppercase;
        outline: 0;
        background: #4caf50;
        width: 100%;
        border: 0;
        padding: 15px;
        color: #fff;
        font-size: 14px;
        -webkit-transition: all 0.3 ease;
        transition: all 0.3 ease;
        cursor: pointer;
      }
      .form button:active,
      .form button:focus,
      .form button:hover {
        background: #43a047;
      }
      .form .message {
        margin: 15px 0 0;
        color: #b3b3b3;
        font-size: 12px;
      }
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
        <form class="config-form" action="http://192.168.4.1/submit" method="post">
          <input type="text" name="pSSID" placeholder="SSID" />
          <input type="password" name="pPassword" placeholder="Password" />
          <input type="text" name="pGateway" placeholder="Gateway" />
          <input type="text" name="pDeviceName" placeholder="Device name" />
          <button>Save</button>
          <p class="message">
            Device Config - <a href="https://github.com/isaacvitor">Isaac Vitor</a>
          </p>
        </form>
      </div>
    </div>
  </body>
</html>
)=====";
