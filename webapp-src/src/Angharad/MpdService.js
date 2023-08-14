import React, { Component } from 'react';
import i18next from 'i18next';

import apiManager from '../lib/APIManager';
import messageDispatcher from '../lib/MessageDispatcher';
import Notification from '../lib/Notification';
import ServiceAdminButtons from './ServiceAdminButtons';

class MpdService extends Component {
  constructor(props) {
    super(props);

    this.state = {
      config: props.config,
      deviceOverview: props.deviceOverview,
      element: props.element,
      adminMode: props.adminMode,
      streamList: props.streamList,
      status: {state: "stop", volume: 0, random: false, repeat: false, song_pos: 0},
      playlists: [],
      curPlaylist: false,
      selectedStream: false,
      selectedMpdPlaylist: false,
      curPos: -1,
      now: false,
      currentStream: false,
      isWebradio: false,
      isTaliesin: false,
      coverBase64: false
    }
    
    this.timeout = false;
    
    this.selectStream = this.selectStream.bind(this);
    this.selectMpdPlaylist = this.selectMpdPlaylist.bind(this);
    this.playSelectedStream = this.playSelectedStream.bind(this);
    this.togglePlayStop = this.togglePlayStop.bind(this);
    this.loopPlayerStatus = this.loopPlayerStatus.bind(this);
    this.sendStreamComand = this.sendStreamComand.bind(this);
    this.setVolumeOff = this.setVolumeOff.bind(this);
    this.adjustVolume = this.adjustVolume.bind(this);
    this.sendMpdCommand = this.sendMpdCommand.bind(this);
    this.openTaliesin = this.openTaliesin.bind(this);
    this.setSwitch = this.setSwitch.bind(this);
    
    this.getPlayerConfig()
    .finally(() => {
      this.loopPlayerStatus();
    });
  }
  
  static getDerivedStateFromProps(props, state) {
    return props;
  }
  
  loopPlayerStatus() {
    this.getPlayerStatus()
    .then(() => {
      let time = 300000;
      if (this.state.status.state !== "stop") {
        time = 10000;
      }
      clearTimeout(this.timeout);
      this.timeout = setTimeout(() => {
        this.loopPlayerStatus();
      }, time);
    });
  }

  componentWillUnmount() {
    clearTimeout(this.timeout);
  }
  
  getPlayerConfig() {
    let proms = [];
    proms.push(apiManager.APIRequestCarleon("service-mpd/" + encodeURIComponent(this.state.element.name) + "/playlists")
    .then(result => {
      this.setState({playlists: result});
    })
    .catch(err => {
      messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
    }));
    proms.push(apiManager.APIRequestCarleon("service-mpd/" + encodeURIComponent(this.state.element.name) + "/playlist")
    .then(result => {
      this.setState({curPlaylist: result});
    })
    .catch(err => {
      messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
    }));
    return Promise.allSettled(proms);
  }
  
  getPlayerStatus() {
    return apiManager.APIRequestCarleon("service-mpd/" + encodeURIComponent(this.state.element.name) + "/status")
    .then(result => {
      return this.setState({status: result}, () => {
        this.updateCurrent();
      });
    })
    .catch(err => {
      messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
    });
  }
  
  updateCurrent() {
    let curPos = this.state.curPos, manageOption = false, stream = false, isWebradio = false, index = -1, isTaliesin = false;
    if (curPos !== this.state.status.song_pos || curPos === -1 || this.state.status.song_pos === -1) {
      if (this.state.status.uri) {
        let statusUri = this.state.status.uri.replace(/([^:]\/)\/+/g, "$1");
        if (statusUri.startsWith(this.state.config.frontend.taliesinRootUrl + "/" + this.state.config.taliesinConfig.api_prefix + "/stream/")) {
          isTaliesin = true;
          stream = statusUri.substring((this.state.config.frontend.taliesinRootUrl + "/" + this.state.config.taliesinConfig.api_prefix + "/stream/").length);
          if (stream.indexOf("?") > -1) {
            stream = stream.substring(0, stream.indexOf("?"));
          }
          if (statusUri.indexOf("index=") > -1) {
            index = parseInt(statusUri.substring(statusUri.indexOf("index=")+"index=".length));
            manageOption = {
              command: "list",
              parameters: {
                limit: 1,
                offset: index
              }
            }
          } else {
            isWebradio = true;
            manageOption = {
              command: "now"
            }
          }
        } else if (statusUri.startsWith(this.state.config.taliesinConfig.icecast_remote_address)) {
          isTaliesin = true;
          isWebradio = true;
          stream = statusUri.substring(this.state.config.taliesinConfig.icecast_remote_address.length + 1);
          if (stream.indexOf("?") > -1) {
            stream = stream.substring(0, stream.indexOf("?"));
          }
          manageOption = {
            command: "now"
          }
        }
      }
      if (stream && manageOption) {
        apiManager.APIRequestTaliesin("stream/" + encodeURIComponent(stream) + "/manage", "PUT", manageOption)
        .then(result => {
          let newNow, updateCover = false;
          if (Array.isArray(result)) {
            newNow = result[0];
          } else {
            newNow = result;
          }
          if (!this.state.now || newNow.data_source !== this.state.now.data_source || newNow.path !== this.state.now.path) {
            updateCover = true;
          }
          this.setState({now: newNow, isWebradio: isWebradio, isTaliesin: isTaliesin, currentStream: stream}, () => {
            if (updateCover) {
              apiManager.APIRequestTaliesin("stream/" + encodeURIComponent(stream) + "/cover?base64" + (index>-1?("&index="+index):""), "GET")
              .then(result => {
                this.setState({coverBase64: result});
              })
              .catch(err => {
                this.setState({coverBase64: false});
              });
            }
          });
        })
        .catch(err => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
        });
      } else {
        this.setState({now: false, isWebradio: false, currentStream: false});
      }
    }
  }
  
  selectStream(e, stream) {
    e.preventDefault();
    this.setState({selectedStream: stream, selectedMpdPlaylist: false});
  }
  
  selectMpdPlaylist(e, playlist) {
    e.preventDefault();
    this.setState({selectedStream: false, selectedMpdPlaylist: playlist});
  }
  
  playSelectedStream() {
    let prom;
    if (this.state.selectedStream) {
      let uri = "";
      if (this.state.selectedStream.icecast) {
        uri = this.state.config.taliesinConfig.icecast_remote_address + "/" + this.state.selectedStream.name;
        prom = apiManager.APIRequestCarleon("service-mpd/" + encodeURIComponent(this.state.element.name) + "/playlist/", "POST", [uri]);
      } else if (this.state.selectedStream.webradio) {
        uri = this.state.config.frontend.taliesinRootUrl + "/" + this.state.config.taliesinConfig.api_prefix + "/stream/" + this.state.selectedStream.name;
        prom = apiManager.APIRequestCarleon("service-mpd/" + encodeURIComponent(this.state.element.name) + "/playlist/", "POST", [uri]);
      } else {
        let playlist = [];
        for (let i=0; i< this.state.selectedStream.elements; i++) {
          playlist.push(this.state.config.frontend.taliesinRootUrl + "/" + this.state.config.taliesinConfig.api_prefix + "/stream/" + this.state.selectedStream.name + "?index=" + i);
        }
        prom = apiManager.APIRequestCarleon("service-mpd/" + encodeURIComponent(this.state.element.name) + "/playlist/" + encodeURIComponent(uri), "POST", playlist);
      }
    } else if (this.state.selectedMpdPlaylist) {
      prom = apiManager.APIRequestCarleon("service-mpd/" + encodeURIComponent(this.state.element.name) + "/playlist/" + encodeURIComponent(this.state.selectedMpdPlaylist), "PUT");
    }
    if (prom) {
      prom.then(() => {
        clearTimeout(this.timeout);
        this.loopPlayerStatus();
        if (this.state.element.device && this.state.element["switch"]) {
          apiManager.APIRequestBenoic("device/" + encodeURIComponent(this.state.element.device) + "/switch/" + encodeURIComponent(this.state.element["switch"]) + "/set/1" , "PUT")
          .catch(err => {
            messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
          });
        }
        apiManager.APIRequestCarleon("service-mpd/" + encodeURIComponent(this.state.element.name) + "/action/play", "PUT")
        .then(result => {
          this.setState({selectedStream: false, selectedMpdPlaylist: false, now: false, curPos: -1, isWebradio: false, coverBase64: false}, () => {
            this.getPlayerStatus();
          });
        })
        .catch(err => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
        });
      })
      .catch(err => {
        messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
      });
    }
  }
  
  togglePlayStop(e) {
    e.preventDefault();
    if (this.state.status.state === "play") {
      if (this.state.element.device && this.state.element["switch"]) {
        apiManager.APIRequestBenoic("device/" + encodeURIComponent(this.state.element.device) + "/switch/" + encodeURIComponent(this.state.element["switch"]) + "/set/0" , "PUT")
        .catch(err => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
        });
      }
      apiManager.APIRequestCarleon("service-mpd/" + encodeURIComponent(this.state.element.name) + "/action/stop", "PUT")
      .then(result => {
        clearTimeout(this.timeout);
        this.loopPlayerStatus();
      })
      .catch(err => {
        messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
      });
    } else {
      if (this.state.element.device && this.state.element["switch"]) {
        apiManager.APIRequestBenoic("device/" + encodeURIComponent(this.state.element.device) + "/switch/" + encodeURIComponent(this.state.element["switch"]) + "/set/1" , "PUT")
        .catch(err => {
          messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
        });
      }
      apiManager.APIRequestCarleon("service-mpd/" + encodeURIComponent(this.state.element.name) + "/action/play", "PUT")
      .then(result => {
        this.setState({selectedStream: false, selectedMpdPlaylist: false}, () => {
          clearTimeout(this.timeout);
          this.loopPlayerStatus();
        });
      })
      .catch(err => {
        messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
      });
    }
  }
  
  sendStreamComand(e, command) {
    e.preventDefault();
    return apiManager.APIRequestTaliesin("stream/" + encodeURIComponent(this.state.currentStream) + "/manage", "PUT", {command: command})
    .then(result => {
      messageDispatcher.sendMessage('Notification', {type: "success", message: i18next.t("services.carleon-service-mpd-command-ok")});
      setTimeout(() => {
        this.updateCurrent();
      }, 1000);
    })
    .catch(err => {
      messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
    });
  }

  buildSongDisplay(now) {
    let outputJsx;
    if (now.tags) {
      outputJsx = [];
      if (now.tags.artist) {
        outputJsx.push(<div key="artist">{now.tags.artist}</div>);
      }
      if (now.tags.title) {
        outputJsx.push(<div key="title">{now.tags.title}</div>);
      }
      if (now.tags.album) {
        outputJsx.push(<div key="album">{now.tags.album}</div>);
      }
      if (now.tags.date) {
        outputJsx.push(<div key="date">({now.tags.date.substring(0, 4)})</div>);
      }
    } else {
      outputJsx = now.name;
    }
    return outputJsx;
  }
  
  setVolumeOff(e) {
    e.preventDefault();
    return apiManager.APIRequestCarleon("service-mpd/" + encodeURIComponent(this.state.element.name) + "/volume/0", "PUT")
    .then(() => {
      let status = this.state.status;
      status.volume = 0;
      this.setState({status: status});
    })
    .catch(err => {
      messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
    });
  }
  
  setSwitch(e, value) {
    return apiManager.APIRequestBenoic("device/" + encodeURIComponent(this.state.element.device) + "/switch/" + encodeURIComponent(this.state.element["switch"]) + "/set/" + value , "PUT")
    .then(() => {
      messageDispatcher.sendMessage('Component', {status: "update", type: "switch", device: this.state.element.device, name: this.state.element["switch"], value: value});
    })
    .catch(err => {
      messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
    });
  }
  
  adjustVolume(e, adjust) {
    e.preventDefault();
    let status = this.state.status;
    status.volume += adjust;
    if (status.volume >= 0 && status.volume <= 100) {
      apiManager.APIRequestCarleon("service-mpd/" + encodeURIComponent(this.state.element.name) + "/volume/" + status.volume, "PUT")
      .then(() => {
        this.setState({status: status});
      })
      .catch(err => {
        messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
      });
    }
  }
  
  sendMpdCommand(e, command) {
    e.preventDefault();
    let param = "";
    if ((command === "random" && !this.state.status.random) || (command === "repeat" && !this.state.status.repeat)) {
      param = "/1";
    }
    if (this.state.element.device && this.state.element["switch"]) {
      if (command === "play") {
        if (this.state.element.device && this.state.element["switch"]) {
          apiManager.APIRequestBenoic("device/" + encodeURIComponent(this.state.element.device) + "/switch/" + encodeURIComponent(this.state.element["switch"]) + "/set/1" , "PUT")
          .catch(err => {
            messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
          });
        }
      } else if (command === "stop") {
        if (this.state.element.device && this.state.element["switch"]) {
          apiManager.APIRequestBenoic("device/" + encodeURIComponent(this.state.element.device) + "/switch/" + encodeURIComponent(this.state.element["switch"]) + "/set/0" , "PUT")
          .catch(err => {
            messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
          });
        }
      }
    }
    return apiManager.APIRequestCarleon("service-mpd/" + encodeURIComponent(this.state.element.name) + "/action/" + command + param, "PUT")
    .then(result => {
      this.getPlayerStatus();
    })
    .catch(err => {
      messageDispatcher.sendMessage('Notification', {type: "danger", message: i18next.t("message.api-error")});
    });
  }
  
  openTaliesin(e) {
    e.preventDefault();
    window.open(this.state.config.frontend.taliesinRootUrl + "#playerCarleon/" + this.state.element.name, "_blank");
  }

	render() {
    let adminButtonsJsx, playList = [], titleJsx, curPlaylistJsx, controlButtonsJsx, switchButtonJsx;
    if (this.state.adminMode) {
      adminButtonsJsx =
        <div className="col-2">
          <ServiceAdminButtons type={"service-mpd"} deviceOverview={this.state.deviceOverview} element={this.state.element} />
        </div>
    }
    this.state.streamList.forEach((stream, index) => {
      playList.push(
        <li key={"tal-"+index}><a className="dropdown-item" href="#" onClick={(e) => this.selectStream(e, stream)}>{stream.display_name}</a></li>
      );
    });
    if (playList.length) {
      playList.push(
        <li key="div"><hr className="dropdown-divider"/></li>
      );
    }
    this.state.playlists.forEach((playlist, index) => {
      playList.push(
        <li key={"mpd-"+index}><a className="dropdown-item" href="#" onClick={(e) => this.selectMpdPlaylist(e, playlist)}>{playlist}</a></li>
      );
    });
    if (this.state.status.state === "play") {
      if (this.state.now) {
        titleJsx = <div className="elt-top mpd-text fs-5">{this.buildSongDisplay(this.state.now)}</div>
      } else {
        titleJsx = <div className="elt-top mpd-text fs-5">{this.state.status.title||this.state.status.name||"-"}</div>
      }
    }
    if (this.state.selectedStream) {
      curPlaylistJsx = this.state.selectedStream.display_name;
    } else if (this.state.selectedMpdPlaylist) {
      curPlaylistJsx = this.state.selectedMpdPlaylist;
    } else if (this.state.currentStream) {
      curPlaylistJsx = i18next.t("services.carleon-service-mpd-playlists");
      this.state.streamList.forEach((stream, index) => {
        if (stream.name === this.state.currentStream) {
          curPlaylistJsx = stream.display_name;
        }
      });
    } else {
      curPlaylistJsx = i18next.t("services.carleon-service-mpd-playlists");
    }
    let style = {};
    if (this.state.coverBase64) {
      style = {
        "backgroundImage": "url('data:image/png;base64," + this.state.coverBase64 + "')"
      }
    }
    let randomClass = "btn-light", repeatClass = "btn-light";
    if (this.state.status.random) {
      randomClass = "btn-secondary";
    }
    if (this.state.status.repeat) {
      repeatClass = "btn-secondary";
    }
    if (this.state.element.device && this.state.element["switch"]) {
      if (this.state.deviceOverview[this.state.element.device] && this.state.deviceOverview[this.state.element.device].switches && this.state.deviceOverview[this.state.element.device].switches[this.state.element["switch"]]) {
        if (this.state.deviceOverview[this.state.element.device].switches[this.state.element["switch"]].value === 1) {
          switchButtonJsx = 
            <button type="button" className="btn btn-secondary" onClick={(e) => this.setSwitch(e, 0)}>
              <i className="fa fa-toggle-on" aria-hidden="true"></i>
            </button>
        } else {
          switchButtonJsx = 
            <button type="button" className="btn btn-secondary" onClick={(e) => this.setSwitch(e, 1)}>
              <i className="fa fa-toggle-off" aria-hidden="true"></i>
            </button>
        }
      } else {
        switchButtonJsx = 
          <button type="button" className="btn btn-secondary" disabled={true}>
            <i className="fa fa-toggle-off" aria-hidden="true"></i>
          </button>
      }
    }
    if (this.state.isWebradio) {
      if (this.state.status.state === "play") {
        controlButtonsJsx =
          <div className="btn-group btn-group-lg" role="group">
            <button type="button" className="btn btn-secondary" onClick={(e) => this.sendStreamComand(e, "replay")}>
              <i className="fa fa-step-backward" aria-hidden="true"></i>
            </button>
            <button type="button" className="btn btn-secondary" onClick={this.togglePlayStop}>
              <i className="fa fa-stop" aria-hidden="true"></i>
            </button>
            <button type="button" className="btn btn-secondary" onClick={(e) => this.sendStreamComand(e, "skip")}>
              <i className="fa fa-step-forward" aria-hidden="true"></i>
            </button>
            {switchButtonJsx}
            <button type="button" className="btn btn-secondary" onClick={this.openTaliesin}>
              <i className="fa fa-external-link-square" aria-hidden="true"></i>
            </button>
          </div>
      } else {
        controlButtonsJsx =
          <div className="btn-group btn-group-lg elt-left" role="group">
            <button type="button" className="btn btn-secondary">
              <i className="fa fa-step-backward" aria-hidden="true" onClick={(e) => this.sendStreamComand(e, "replay")}></i>
            </button>
            <button type="button" className="btn btn-secondary" onClick={this.togglePlayStop}>
              <i className="fa fa-play" aria-hidden="true"></i>
            </button>
            <button type="button" className="btn btn-secondary" onClick={(e) => this.sendStreamComand(e, "skip")}>
              <i className="fa fa-step-forward" aria-hidden="true"></i>
            </button>
            {switchButtonJsx}
            <button type="button" className="btn btn-secondary" onClick={this.openTaliesin}>
              <i className="fa fa-external-link-square" aria-hidden="true"></i>
            </button>
          </div>
      }
    } else {
      let playButton = "";
      if (this.state.status.state === "play") {
        playButton =
          <button type="button" className="btn btn-secondary" onClick={(e) => this.sendMpdCommand(e, "pause")}>
            <i className="fa fa-pause" aria-hidden="true"></i>
          </button>
      } else if (this.state.status.state === "pause" || this.state.status.state === "stop") {
        playButton =
          <button type="button" className="btn btn-secondary" onClick={(e) => this.sendMpdCommand(e, "play")}>
            <i className="fa fa-play" aria-hidden="true"></i>
          </button>
      }
      controlButtonsJsx =
        <div className="btn-group btn-group-lg" role="group">
          <button type="button" className="btn btn-secondary" onClick={(e) => this.sendMpdCommand(e, "previous")}>
            <i className="fa fa-step-backward" aria-hidden="true"></i>
          </button>
          <button type="button" className="btn btn-secondary" onClick={(e) => this.sendMpdCommand(e, "stop")}>
            <i className="fa fa-stop" aria-hidden="true"></i>
          </button>
          {playButton}
          <button type="button" className="btn btn-secondary" onClick={(e) => this.sendMpdCommand(e, "next")}>
            <i className="fa fa-step-forward" aria-hidden="true"></i>
          </button>
          {switchButtonJsx}
          <button type="button" className={"btn "+randomClass} onClick={(e) => this.sendMpdCommand(e, "random")}>
            <i className="fa fa-random" aria-hidden="true"></i>
          </button>
          <button type="button" className={"btn "+repeatClass} onClick={(e) => this.sendMpdCommand(e, "repeat")}>
            <i className="fa fa-repeat" aria-hidden="true"></i>
          </button>
        </div>
    }
    let disabledMute = "", disabledPlus5 = "", disabledPlus1 = "", disabledMinus1 = "", disabledMinus5 = "";
    if (this.state.status.volume <= 0) {
      disabledMute = " disabled";
      disabledMinus1 = " disabled";
      disabledMinus5 = " disabled";
    } else if (this.state.status.volume < 5) {
      disabledMinus5 = " disabled";
    } else if (this.state.status.volume > 95) {
      disabledPlus5 = " disabled";
    } else if (this.state.status.volume >= 100) {
      disabledPlus5 = " disabled";
      disabledPlus1 = " disabled";
    }
		return (
      <div className="row border rounded elt-top mpd-element agrd-element mpd-cover" style={style}>
        <div className="col-sm text-center">
          <div className="row">
            <div className="col-2 text-center">
              <label className="d-flex align-items-center mpd-text">
                <i className="fa fa-music elt-left" aria-hidden="true">
                </i>
                {this.state.element.name}
              </label>
            </div>
            <div className="col-8">
            </div>
            <div className="col-2">
              {adminButtonsJsx}
            </div>
          </div>
          <div className="btn-group" role="group" aria-label="Basic example">
            <div className="btn-group" role="group">
              <button className="btn btn-secondary dropdown-toggle" type="button" data-bs-toggle="dropdown" aria-expanded="false">
                {curPlaylistJsx}
              </button>
              <ul className="dropdown-menu">
                {playList}
              </ul>
            </div>
            <button type="button" className="btn btn-secondary" onClick={this.playSelectedStream}>
              <i className="fa fa-play-circle" aria-hidden="true"></i>
            </button>
            <div className="btn-group" role="group">
              <button type="button" className="btn btn-secondary dropdown-toggle" data-bs-toggle="dropdown" aria-expanded="false">
                <i className="fa fa-volume-up elt-left" aria-hidden="true"></i>
                {this.state.status.volume}%
              </button>
              <ul className="dropdown-menu">
                <li>
                  <a className={"dropdown-item"+disabledMute} href="#" onClick={this.setVolumeOff} disabled={this.state.volume === 0}>
                    <i className="fa fa-volume-mute elt-left" aria-hidden="true"></i>
                    {i18next.t("services.carleon-service-mpd-volume-mute")}
                  </a>
                </li>
                <li>
                  <a className={"dropdown-item"+disabledPlus5} href="#" onClick={(e) => this.adjustVolume(e, 5)}>
                    <i className="fa fa-volume-down elt-left" aria-hidden="true"></i>
                    {i18next.t("services.carleon-service-mpd-volume-plus-5")}
                  </a>
                </li>
                <li>
                  <a className={"dropdown-item"+disabledPlus1} href="#" onClick={(e) => this.adjustVolume(e, 1)}>
                    <i className="fa fa-volume-down elt-left" aria-hidden="true"></i>
                    {i18next.t("services.carleon-service-mpd-volume-plus-1")}
                  </a>
                </li>
                <li>
                  <a className={"dropdown-item"+disabledMinus1} href="#" onClick={(e) => this.adjustVolume(e, -1)}>
                    <i className="fa fa-volume-down elt-left" aria-hidden="true"></i>
                    {i18next.t("services.carleon-service-mpd-volume-minus-1")}
                  </a>
                </li>
                <li>
                  <a className={"dropdown-item"+disabledMinus5} href="#" onClick={(e) => this.adjustVolume(e, -5)}>
                    <i className="fa fa-volume-down elt-left" aria-hidden="true"></i>
                    {i18next.t("services.carleon-service-mpd-volume-minus-5")}
                  </a>
                </li>
              </ul>
            </div>
          </div>
          <div className="text-center elt-top elt-bottom">
            {controlButtonsJsx}
          </div>
          {titleJsx}
        </div>
      </div>
		);
	}
}

export default MpdService;
