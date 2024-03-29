import React, { Component } from 'react';

import messageDispatcher from './MessageDispatcher';

class Notification extends Component {
  constructor(props) {
    super(props);
    this.state = {
      message: [],
      counter: 0,
      notHidden: [],
      loggedIn: props.loggedIn
    }

    messageDispatcher.subscribe('Notification', (message) => {
      if (message.type) {
        var myMessage = this.state.message;
        myMessage.push({type: message.type, message: message.message, id: this.state.counter});
        this.setState({message: myMessage, counter: this.state.counter+1}, () => {
          var autohide = message.autohide;
          if (autohide === undefined) {
            autohide = true;
          }
          let toast = $("#toast-"+(this.state.counter-1)).toast({animation: true, autohide: autohide}).toast('show');
          if (!autohide) {
            let notHidden = this.state.notHidden;
            notHidden.push(toast);
            this.setState({notHidden: notHidden});
          }
        });
      } else if (message.hideAll) {
        this.state.notHidden.forEach(toast => {
          toast.toast('hide');
          this.setState({notHidden: []});
        });
      }
    });
    
    this.close = this.close.bind(this);
  }
  
  static getDerivedStateFromProps(props, state) {
    return props;
  }

  close(id) {
    var myMessages = this.state.message;
    myMessages.forEach((message, index) => {
      if (message.id === id) {
        myMessages.splice(index, 1);
        this.setState({message: myMessages});
      }
    });
  }
  
  render() {
    var toast = [];
    var showMessage = this.state.loggedIn;
    this.state.message.forEach((message) => {
      if (message.type === "danger" || message.type === "warning") {
        showMessage = true;
      }
    });
    if (showMessage) {
      this.state.message.forEach((message, index) => {
        var icon;
        if (message.type === "success") {
          icon = <i className="fa fa-check-square-o elt-left text-success"></i>;
        } else if (message.type === "danger") {
          icon = <i className="fa fa-exclamation-circle elt-left text-danger"></i>;
        } else if (message.type === "warning") {
          icon = <i className="fa fa-exclamation-triangle elt-left text-warning"></i>;
        } else { // info
          icon = <i className="fa fa-info-circle elt-left text-info"></i>;
        }
        toast.push(
          <div className="toast" role="alert" aria-live="assertive" aria-atomic="true" key={index} id={"toast-"+message.id}>
            <div className="toast-header">
              {icon}
              <strong className="me-auto">Angharad</strong>
              <button aria-label="Close" className="btn-close" data-bs-dismiss="toast" type="button" onClick={(e) => this.close(message.id)}>
              </button>
            </div>
            <div className="toast-body">
              {message.message}
            </div>
          </div>
        );
      });
    }
    return (
      <div className="position-fixed" style={{top: 45, right: 20, zIndex: 9999}}>
        {toast}
      </div>
    );
  }
}

export default Notification;
